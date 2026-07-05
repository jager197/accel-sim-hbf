# HBF-Sim 项目概述

## 背景

HBF（High Bandwidth Flash）是 SanDisk 和 SK hynix 推动的新型闪存，把 3D NAND 通过 TSV 堆叠在 logic die 上，直接放在 GPU interposer 上跟 HBM 一起用。它把 NAND 阵列拆成数千个独立微子阵列，通过 logic die 并行调度，聚合带宽接近 HBM，容量是 HBM 的 8-16 倍，成本仅为 HBM 的 1/8。

目前 HBF 还没有可用的硬件（首批样片预计 2026 下半年），学术界也缺乏开源的模拟验证平台。本项目填补这一空白，基于 Accel-Sim/GPGPU-Sim 构建 HBF 模拟器。

## 项目地址

https://github.com/jager197/accel-sim-hbf

## 整体架构

HBF 模块插在 GPGPU-Sim 的 `memory_partition_unit` 层，与 DRAM 控制器并列：

```
SM Core → L1 → Interconnect → L2 Cache
                                  │
                            L2 miss
                                  │
                    ┌─────────────┴─────────────┐
                    │   is_hbf_addr(addr)?       │
                    └─────────────┬─────────────┘
                          │               │
                    HBF range        DRAM range
                          │               │
                   hbf_controller    dram_t (原有)
                          │               │
                    └───────┬───────────────┘
                            │
                      returnq → L2 fill → SM
```

地址路由通过 `memory_config::is_hbf_addr()` 实现——超出一个可配置基地址的请求走 HBF，其余走 DRAM。同时提供了 `hbf_route_all` 测试模式，可将所有 L2 miss 强制路由到 HBF。

---

## v0.1 — 固定延迟 HBF

第一阶段实现了一个简单的固定延迟 HBF 控制器，验证地址路由和集成路径。

**核心类：** `hbf_ctrl_t`（位于 `hbf/` 目录，约 120 行）

```cpp
// 接口照搬 dram_t，push → 延迟 FIFO → cycle → 返回队列
void hbf_ctrl_t::push(mem_fetch *data) {
    hbf_delay_t d;
    d.req = data;
    d.ready_cycle = now + hbf_latency;  // 固定延迟
    m_latency_queue.push_back(d);
}

void hbf_ctrl_t::cycle() {
    // 到期的请求搬到返回队列
    while (!empty && now >= front.ready_cycle) {
        m_returnq->push(front.req);
        pop_front();
    }
}
```

**改动点：** 修改 `l2cache`（增加 HBF 控制器和路由）、`gpu-sim`（增加 5 个配置项和主循环调用），共约 150 行增量。

**验证：** PTX 模式功能正确（vectorAdd PASS），trace 驱动模式下 pathfinder benchmark 产生 512 次 HBF 读。

---

## v0.2 — 逐周期精确 NAND 控制器

第二阶段用真正的 NAND flash 控制器替换了固定延迟 FIFO。三个核心组件：

### 1. 子阵列状态机（`hbf_subarray_t`）

参照 dram_t 的 `bank_t` 设计，每个子阵列有四种状态：

```cpp
enum state_t { IDLE, READING, PROGRAMMING, ERASING };

void start_read(page, block)  { state = READING;    counter = tR;    }
void start_program(page, block) { state = PROGRAMMING; counter = tPROG; }
void start_erase(block)       { state = ERASING;    counter = tBERS; }

void cycle() {
    if (counter > 0) counter--;
    if (counter == 0) state = IDLE;  // 操作完成
}
```

### 2. MSHR 合并（`hbf_controller_t`）

这是 HBF 区别于普通 DRAM 延迟模型的核心——多个 64B cache line 请求合并为一次 NAND page 读：

```cpp
void hbf_controller_t::push(mem_fetch *data) {
    page = addr / page_size;
    if (m_mshr.find(page) != end) {
        // 同 page 已有请求在飞 → 合并，不发起新的 page 读
        m_mshr[page].pending.push_back(data);
        n_mshr_hits++;
    } else {
        // 新 page → 创建 MSHR 条目，排队等待空闲子阵列
        m_mshr[page] = new_entry;
    }
}
```

每周期 `cycle()` 将 MSHR 队列中的条目分配给空闲子阵列，子阵列完成操作后将所有 pending 请求的数据一次性返回。

### 3. FTL（`hbf_ftl_t`）

简化的页级映射表，提供逻辑页到物理地址的转换和 GREEDY 垃圾回收：

```cpp
hbf_phys_addr_t translate(logical_page, is_write) {
    if (is_write) invalidate(logical_page);  // 写前作废旧映射
    // 分配新物理页
    return allocate_block(subarray)->next_free_page();
}

void gc() {
    victim = block_with_fewest_valid_pages();  // GREEDY 策略
    copy_valid_pages(victim);                  // 搬移有效数据
    erase(victim);                             // 擦除后回收到空闲池
}
```

**改动点：** 新增 6 个文件（约 870 行），修改 gpu-sim 配置（增加 10 个 Phase 2 选项）。

**实验验证（pathfinder trace, MSHR=1 vs MSHR=0）：**

|                        | MSHR=1 | MSHR=0 |
|------------------------|--------|--------|
| 总请求数               | 16     | 16     |
| NAND page 读           | **4**  | 16     |
| MSHR 合并率            | **75%**| 0%     |

MSHR 将 16 个 cache line 请求合并为 4 次 NAND page 读，读放大降低 4 倍。

---

## 快速开始

```bash
git clone git@github.com:jager197/accel-sim-hbf.git
cd accel-sim-hbf
export CUDA_INSTALL_PATH=/usr/local/cuda
source ./gpu-simulator/setup_environment.sh
bash setup_hbf.sh
make -j$(nproc) -C ./gpu-simulator

# 功能测试
bash run_smoke_test.sh

# 下载预录 trace 并验证 HBF 流量
mkdir hw_run && cd hw_run
curl -LO https://engineering.purdue.edu/tgrogers/accel-sim/traces/tesla-v100/latest/rodinia_2.0-ft.tgz
tar xzf rodinia_2.0-ft.tgz && cd ..
# 使用 HBF 配置运行 trace-driven 模拟
./gpu-simulator/bin/release/accel-sim.out \
  -trace hw_run/rodinia_2.0-ft/9.1/pathfinder-.../traces/kernelslist.g \
  -config hbf/gpgpusim_hbf.config \
  -config gpu-simulator/configs/tested-cfgs/SM7_QV100/trace.config
```

## 参考文献

- **Accel-Sim** (ISCA 2020) — [accel-sim/accel-sim-framework](https://github.com/accel-sim/accel-sim-framework)
- **GPGPU-Sim** — [gpgpu-sim/gpgpu-sim_distribution](https://github.com/gpgpu-sim/gpgpu-sim_distribution)
- **MQSim** (FAST 2018) — [CMU-SAFARI/MQSim](https://github.com/CMU-SAFARI/MQSim)
- **CXL-MQSim** (USENIX ATC 2023) — [sang-jun-kim/CXL-MQSim](https://github.com/sang-jun-kim/CXL-MQSim)
