# HBF-Sim 项目概述

> 基于 Accel-Sim/GPGPU-Sim 的 HBF（High Bandwidth Flash）模拟器

## 背景

HBF 是 SanDisk 和 SK hynix 正在推动的一种新型闪存。它把 3D NAND 通过 TSV 堆叠在 logic die 上，直接放在 GPU interposer 上跟 HBM 一起用。HBF 把 NAND 阵列拆成数千个独立微子阵列，通过 logic die 并行调度，聚合带宽接近 HBM，容量是 HBM 的 8-16 倍，成本仅为 HBM 的 1/8。

目前 HBF 还没有可用的硬件（首批样片预计 2026 下半年），学术界也缺乏开源的模拟验证平台。本项目填补这一空白，基于 Accel-Sim/GPGPU-Sim 构建 HBF 模拟器。

项目地址：https://github.com/jager197/accel-sim-hbf

## 整体架构

HBF 模块插在 GPGPU-Sim 的 `memory_partition_unit` 层，与 DRAM 控制器并列：

```
SM Core → L1 → Interconnect → L2 Cache
                                  │
                            L2 miss
                                  │
                    ┌─────────────┴─────────────┐
                    │   is_hbf_addr(addr)?      │
                    └─────────────┬─────────────┘
                          │               │
                    HBF range        DRAM range
                          │               │
               hbf_controller_t      dram_t (原有)
                          │               │
                    └───────┬───────────────┘
                            │
                      returnq → L2 fill → SM
```

地址路由通过 `memory_config::is_hbf_addr()` 实现——超出一个可配置基地址的请求走 HBF，其余走 DRAM。同时提供了 `hbf_route_all` 测试模式，可将所有 L2 miss 强制路由到 HBF，方便验证。

---

## v0.1 — 固定延迟控制器

第一阶段实现了基本的 HBF 内存层，验证地址路由和集成路径。核心类 `hbf_ctrl_t` 约 120 行，接口完全照搬 GPGPU-Sim 自带的 `dram_t`。

**核心逻辑——固定延迟 FIFO**：

```cpp
void push(mem_fetch *data) {
    // 请求进延迟队列，记到期时间 = now + hbf_latency
    queue.push({data, now + hbf_latency});
}

void cycle() {
    // 到期的请求搬到返回队列，送回 L2
    while (!queue.empty() && now >= front.ready_cycle) {
        returnq->push(front.req);
        pop();
    }
}
```

**改动点**：`l2cache`（增加 HBF 控制器和 `hbf_cycle()` 路由方法）、`gpu-sim`（增加 5 个配置项、主循环中调用 `hbf_cycle()`），共约 150 行增量。

---

## v0.2 — 逐周期 NAND 控制器

第二阶段把固定延迟 FIFO 替换为真正的 NAND 闪存控制器，建模了 HBF 区别于 DRAM 的几个关键特性。

### 2.1 多子阵列并行访问

HBF 的核心架构特点是数千个子阵列可以**同时访问**。每个子阵列同一时间只能做一件事（读/写/擦除），但不同子阵列之间完全独立、互不阻塞。

实现方式——参照 dram_t 的 `bank_t` 设计 `hbf_subarray_t`：

```cpp
class hbf_subarray_t {
    enum state_t { IDLE, READING, PROGRAMMING, ERASING };
    state_t m_state;
    unsigned m_timing_remaining;  // 剩余周期数

    void start_read(p, b)   { state = READING;    remaining = tR;    }
    void start_program(p,b) { state = PROGRAMMING; remaining = tPROG; }
    void start_erase(b)     { state = ERASING;    remaining = tBERS; }

    void cycle() {
        if (--remaining == 0) state = IDLE;  // 操作完成
    }
};
```

控制器持有子阵列数组，每周期调度：

```cpp
class hbf_controller_t {
    hbf_subarray_t *m_subarrays[N];  // 数千个子阵列
    unsigned m_max_active;           // 功率限制：同时最多多少个活跃

    void cycle() {
        // 1. 驱动所有子阵列走一拍（递减计时器，完成的变 IDLE）
        for (auto sa : m_subarrays) sa->cycle();
        // 2. 把排队的请求分配给空闲子阵列
        schedule_operations();
    }
};
```

**关键逻辑在 `schedule_operations()`**——遍历等待队列中的请求，找到空闲子阵列就分配：

```
MSHR 队列: [page_A, page_B, page_C, ...]
              │
              ▼ 找空闲子阵列
子阵列[0]: READING (忙)
子阵列[1]: IDLE    (闲) ← 分配 page_A
子阵列[2]: PROGRAMMING (忙)
子阵列[3]: IDLE    (闲) ← 分配 page_B
...
最多同时活跃 m_max_active 个（功率上限）
```

每个子阵列的读/写/擦除操作独立计时，互不等待。并行度由 `m_max_active` 控制，模拟 power budget 约束下的最大并发数。

### 2.2 写路径——写缓冲 + 写前擦除 + MSHR

NAND Flash 的物理特性决定了写入不是简单的"发一个 write 命令"：

- **写前必须擦除**：NAND 写入（program）只能把 bit 从 1 翻到 0；要把 0 翻回 1，必须对整个 block（通常 64 个 page）进行 erase。erase 很慢（~2ms），且擦除以 block 为单位
- **页级写入**：program 粒度是 4KB page，不是 64B cache line

因此写路径设计了三层：

**第一层——写缓冲（Write Buffer）**：

多个 store 写到同一 page 时先合并，再一起交给下一层。避免每个 64B store 都触发一次 page program。

```cpp
// push() 中：写请求先进 write buffer
if (data->is_write()) {
    m_write_buffer[page].push(data);
    if (buffer_full || read_to_same_page) flush_to_mshr(page);
}
```

**第二层——MSHR（Miss Status Holding Register）**：

MSHR 解决的是 GPU 的 64B 访问粒度与 NAND 的 4KB page 粒度不匹配的问题。同 page 的多个请求被合并成一个 MSHR 条目，等子阵列完成 page 读/写后一次性返回所有请求的数据。MSHR 可以开关（`hbf_mshr_enabled`），关闭时每个请求独立发一次 page 操作。

**第三层——写前擦除（Erase-Before-Write）**：

MSHR 条目进入调度后，对于写请求先检查目标 block 是否已擦除：

```
写入请求到达 schedule_operations()
    │
    ├── is_block_erased(block)?
    │       │
    │       ├── YES → start_program()  [tPROG ≈ 200µs]
    │       │           完成后 → 返回数据，删除 MSHR 条目
    │       │
    │       └── NO  → start_erase()    [tBERS ≈ 2ms]
    │                 标记 block 状态为"擦除中"
    │                 子阵列完成 → mark_block_erased(block)
    │                 该 MSHR 条目重新排队
    │                 下一周期 → is_block_erased = YES → start_program()
```

关键实现细节——在 `cycle()` 中区分"刚完成的是 erase 还是 read/program"：

```cpp
// cycle() 中处理子阵列完成事件
if (completed_operation == ERASING) {
    m_ftl->mark_block_erased(subarray, block);
    // MSHR 条目回到 WAITING 状态，下一轮调度时会走 program
} else {
    // READ 或 PROGRAM 完成 → 返回数据给所有 pending 请求
    for (auto req : mshr_entry.pending) {
        req->set_reply();
        returnq->push(req);
    }
    m_mshr.erase(entry);
}
```

### 2.3 FTL——页映射与垃圾回收

NAND 不能原地覆盖写入，每次写入都要分配一个新 page，旧 page 标记为无效。FTL 用一张映射表管理这个逻辑页到物理页的对应关系：

```cpp
// 逻辑页 → (子阵列, block, page) 的映射
map<logical_page, hbf_phys_addr_t> m_mapping;

hbf_phys_addr_t translate(logical_page, is_write) {
    if (is_write) invalidate(logical_page);  // 写前作废旧映射
    // 从当前活跃 block 按顺序分配空闲 page
    return active_block->next_free_page();
}

void invalidate(logical_page) {
    // 逻辑页被覆盖 → 对应物理页标记无效，有效页计数 -1
    block[mapping[logical_page].block].valid_pages--;
    m_mapping.erase(logical_page);
}
```

当空闲 block 数量低于 `overprovisioning` 阈值时触发 GC。GC 采用 GREEDY 策略：

1. 遍历所有 block，找**有效页最少**的那个作为受害者
2. 把受害者 block 中的有效页拷贝到新 block
3. 擦除受害者 block，回收到空闲池

这样写路径的整体流程是：`写缓冲 → MSHR → FTL 分配新 page → 检查 block 是否已擦除 → PROGRAM（或先 ERASE 再 PROGRAM）`。

---

## 快速开始

```bash
git clone git@github.com:jager197/accel-sim-hbf.git
cd accel-sim-hbf
export CUDA_INSTALL_PATH=/usr/local/cuda
source ./gpu-simulator/setup_environment.sh
bash setup_hbf.sh
make -j$(nproc) -C ./gpu-simulator
bash run_smoke_test.sh
```

## 参考文献

- **Accel-Sim** (ISCA 2020) — [accel-sim/accel-sim-framework](https://github.com/accel-sim/accel-sim-framework)
- **GPGPU-Sim** — [gpgpu-sim/gpgpu-sim_distribution](https://github.com/gpgpu-sim/gpgpu-sim_distribution)
- **MQSim** (FAST 2018) — [CMU-SAFARI/MQSim](https://github.com/CMU-SAFARI/MQSim)
- **CXL-MQSim** (USENIX ATC 2023) — [sang-jun-kim/CXL-MQSim](https://github.com/sang-jun-kim/CXL-MQSim)
