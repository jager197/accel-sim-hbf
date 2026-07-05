# HBF-Sim 项目概述

> 基于 Accel-Sim/GPGPU-Sim 的 HBF（High Bandwidth Flash）模拟器

## 1. 背景与动机

### HBF 是什么

HBF 是 SanDisk 和 SK hynix 正在推动的一种新型存储技术。核心思路：把 3D NAND Flash 通过 TSV（硅通孔）堆叠在 logic die 上，直接放在 GPU 的 interposer 上，跟 HBM 做邻居。

区别于传统 SSD 的关键创新在于架构：传统 SSD 把 NAND 组织成十几个通道（channel），每个通道上多个芯片共享一条串行总线（ONFi，~333MB/s per channel）。HBF 把 NAND 阵列拆成**数千个独立微子阵列**，每个子阵列都有专用 I/O 路径直连 logic die。Logic die 同时并行访问大量子阵列，聚合带宽匹配 HBM（数百 GB/s）。

| | HBM | HBF |
|---|---|---|
| 技术 | DRAM | 3D NAND Flash |
| 单 stack 容量 | 16-24 GB | 512 GB |
| 读延迟 | ~100 ns | ~10-75 µs |
| 成本/GB | 极高 | ~HBM 的 1/8 |
| 写耐久度 | 无限 | 有限 P/E cycles |

### 为什么要做这个项目

HBF 目前处于早期验证阶段，预计 2026 下半年才有首批样片，2027 年才有商用产品。学术界有几个组在做 HBF 相关研究，但**全部使用内部模拟器，没有任何开源方案**。我的博士课题涉及 HBF 集成的 GPU 架构研究，缺少可用的模拟平台，所以决定基于 Accel-Sim/GPGPU-Sim 搭建一个。

---

## 2. 整体架构

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
               hbf_controller_t     dram_t (原有)
                          │               │
                    └───────┬───────────────┘
                            │
                      returnq → L2 fill → SM
```

- 地址路由通过 `is_hbf_addr()` 实现：HBF 默认占用 256GB-768GB 地址段
- `hbf_route_all` 测试模式可将所有 L2 miss 强制路由到 HBF
- DRAM 路径增加反向过滤：`if (is_hbf_addr(addr)) continue;` 跳过 HBF 地址
- 接口完全照搬 `dram_t`，一个 `memory_partition_unit` 同时持有 DRAM 和 HBF 两个控制器

---

## 3. MSHR：从 cache-line 到 NAND page 的桥梁

### MSHR 是什么

MSHR（Miss Status Holding Register）是现代缓存控制器中的标准组件。当多个 load/store 请求访问同一个未命中的 cache line 时，MSHR 将后续请求**合并**到已有的 miss 处理流程中，避免重复发送相同的内存请求。

### 为什么 HBF 需要 MSHR

这是 HBF 区别于普通 DRAM 延迟模型的核心。GPU 的访存粒度是 **64B cache line**，而 NAND Flash 的读写粒度是 **4KB page**。如果不做合并：

- 64 个 cache line 请求到同一个 4KB page → 64 次 NAND page read → 64 × 15µs = 960µs
- 有了 MSHR 合并 → 1 次 NAND page read + 63 次等待 → ~15µs

MSHR 是 HBF **用 µs 级延迟匹配 HBM 带宽**的关键机制。

### 实现方式

```cpp
void hbf_controller_t::push(mem_fetch *data) {
    page = addr / page_size;  // 64B地址 → 4KB page地址
    auto it = m_mshr.find(page);
    if (it != m_mshr.end()) {
        // 同 page 已有请求在飞 → 合并，不发起新的 NAND 读
        it->pending.push_back(data);
        n_mshr_hits++;
    } else {
        // 新 page → 创建 MSHR 条目，排队等待空闲子阵列
        m_mshr[page] = new_entry;
        m_mshr_queue.push(page);
    }
}
```

子阵列完成 page 读取后，所有 pending 的 cache line 请求一次性返回。MSHR 可以开关（`hbf_mshr_enabled`），便于对比实验。

---

## 4. 版本演进

### v0.1 — 固定延迟 HBF

验证地址路由和集成路径。核心类 `hbf_ctrl_t`（约 120 行）实现一个固定延迟 FIFO：

```cpp
void push(mem_fetch *data) { queue.push({data, now + latency}); }
void cycle() { while (front.ready) return_queue.push(front.req); }
```

**验证结果**：PTX 模式 vectorAdd PASS，trace 驱动 pathfinder 产生 512 次 HBF 读。

### v0.2 — 逐周期精确 NAND 控制器

核心组件：

**子阵列状态机**（`hbf_subarray_t`，参照 dram_t 的 `bank_t`）：
```cpp
enum state_t { IDLE, READING, PROGRAMMING, ERASING };
void start_read(p, b)   { state = READING;    counter = tR;    }
void start_program(p,b) { state = PROGRAMMING; counter = tPROG; }
void start_erase(b)     { state = ERASING;    counter = tBERS; }
void cycle() { if (--counter == 0) state = IDLE; }
```
每个子阵列同一时间只能执行一种操作，完成后自动回到 IDLE。`max_active` 参数限制同时活跃的子阵列数量（模拟功率上限）。

**MSHR 合并**：已在上节详述。

**写路径——写前擦除**：
NAND 的物理特性决定了：写入（program）只能将 bit 从 1 翻到 0；要将 0 翻回 1，必须以 block 为单位整体擦除（erase）。所以写流程是两步：

```
写入请求 → 写缓冲(合并同page写入)
         → MSHR条目 → schedule_operations()
         → is_block_erased()?
              ├─ YES → start_program()  [~200µs]
              └─ NO  → start_erase()    [~2ms]
                       → 子阵列完成 → mark_block_erased()
                       → 自动重新调度 → start_program()
```

写缓冲在 MSHR 之前增加了一层：同 page 的多次写入先在 buffer 中合并，缓冲满（32 条）或超时（1000 周期）后一次性刷新到 MSHR，减少 NAND program 次数。

**FTL（Flash Translation Layer）**：简化的页级映射表，负责逻辑页→物理地址转换、block 分配、GREEDY 垃圾回收。

---

## 5. 实验验证

### MSHR 合并效果（pathfinder trace, 16 sub-arrays）

|                        | MSHR=1 | MSHR=0 |
|------------------------|--------|--------|
| 总请求数               | 16     | 16     |
| NAND page 读           | **4**  | 16     |
| MSHR 合并率            | **75%**| 0%     |
| 平均操作延迟           | 56 cycles | 44 cycles |

MSHR 将 16 个 cache line 请求合并为 4 次 NAND page 读。75% 的合并率意味着读放大降低了 4 倍。

### FTL 功能验证

```
FTL Translations:   4
FTL Allocations:    2
FTL GC Events:      0
FTL Total Blocks:   1
```

FTL 正常工作：为 4 个 page 访问分配了 2 个物理 block。

---

## 6. 项目地址与快速开始

```bash
git clone git@github.com:jager197/accel-sim-hbf.git
cd accel-sim-hbf
export CUDA_INSTALL_PATH=/usr/local/cuda
source ./gpu-simulator/setup_environment.sh
bash setup_hbf.sh && make -j$(nproc) -C ./gpu-simulator
bash run_smoke_test.sh
```

预录 trace 可从 [Purdue trace server](https://engineering.purdue.edu/tgrogers/accel-sim/traces/tesla-v100/latest/rodinia_2.0-ft.tgz) 下载，用 `accel-sim.out` 回放。

## 7. 参考文献

- **Accel-Sim** (ISCA 2020) — GPU trace-driven 模拟框架
- **GPGPU-Sim** — GPU 时序模型内核
- **MQSim** (FAST 2018) — SSD 模拟器，NAND 时序参数参考
- **CXL-MQSim** (USENIX ATC 2023) — CXL 闪存扩展，概念最接近的项目
