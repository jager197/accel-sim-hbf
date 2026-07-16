# HBF-Sim

基于 Accel-Sim/GPGPU-Sim 构建的 HBF（High Bandwidth Flash）模拟器。

## 背景

HBF 是 SanDisk 和 SK hynix 正在联合研发的新型闪存技术。其思路是将 3D NAND 芯片通过 TSV 堆叠在逻辑芯片（logic die）之上，然后将整个封装直接放置在 GPU interposer 上与 HBM 并列。与传统 SSD 不同，HBF 把 NAND 阵列拆分为数千个可独立寻址的微子阵列（micro sub-array），由 logic die 统一调度并行访问，聚合带宽接近 HBM，容量是 HBM 的 8–16 倍，成本仅为 HBM 的 1/8。

HBF 目前仍在早期阶段——首批样片预计 2026 下半年出货，商用产品预计 2027 年。少数学术团队在做相关研究（如 HAVEN），但均使用内部模拟器，尚无开源方案。

本人的研究涉及 HBF 集成的 GPU 架构。因缺乏公开的仿真平台，于是自行构建。

## 版本

### v0.3（当前）— FTL GC 重映射 + 延迟建模 + 磨损均衡 + Page Cache

修复 v0.2 中 GC 的正确性缺陷，新增逐周期延迟建模，引入磨损均衡和共享 page cache：

- **GC 页重映射** — victim block 中的有效逻辑页在 GC 时正确重映射到新物理页，修复了旧版中 GC 后映射悬空导致读请求拿到错误数据的问题。通过 `translate()`/`invalidate()` 中维护的反向索引 `(subarray, block) → 逻辑页集合` 实现 O(1) 查找 victim 中的有效页。
- **GC 延迟建模** — 每搬移一个有效页计 `tPROG` 周期。FTL 新增 `cycle()` 驱动的 GC 状态机，跟踪 GC stall 周期。victim block 的擦除延迟（`tBERS`）已由现有"写前擦除"路径覆盖，无需额外建模。
- **磨损均衡**（`-gpgpu_hbf_wear_leveling_enabled 1`）— 记录每个 block 的物理擦除次数。开启后，block 分配从空闲池中选擦除次数最少的，GC victim 选择使用 cost function 惩罚高于平均擦除次数的热 block，使磨损均匀分布。统计输出包含 min/avg/max 擦除次数和磨损决策计数。
- **共享 page cache**（`-gpgpu_hbf_cache_entries 256`）— logic die 上的 LRU 读缓存，位于 MSHR 与子阵列调度之间。读命中仅需 `hbf_cache_hit_latency`（~50 cycles）而非完整 tR（15,000 cycles）。写 invalidate 策略。
- **子阵列 page buffer** — 建模 NAND page register（每子阵列 1 page）。连续读同一页命中 register 时仅需 `hbf_buffer_hit_latency` 周期。
- **新增统计** — `GC Stall Cycles`、`Avg GC Latency`、`Erase Count` min/avg/max、磨损均衡决策计数、`Page Buffer Hits`、`Page Cache` 命中率。
- **新增文件/方法** — `hbf_page_cache.h/cc`、`hbf_ftl_t::cycle()`、`hbf_ftl_t::allocate_page_for_gc()`、磨损感知分配/GC、page buffer 逻辑。

### v0.2 — 逐周期 NAND 控制器

在 v0.1 基础上，用真正的闪存控制器替换固定延迟 FIFO：

- **NAND 子阵列状态机** — IDLE → READING → PROGRAMMING → ERASING，每种操作有可配置的延迟（tR / tPROG / tBERS），参照 GPGPU-Sim DRAM 模型中 `bank_t` 的设计模式。
- **MSHR 合并** — 多个 64B cache-line 请求合并为一次 4KB NAND page 读取。在 rodinia trace 上验证：75% 合并率（16 个请求 → 4 次 page read）。
- **功率约束** — 可配置的最大同时活跃子阵列数（`hbf_max_active`）。
- **页级 FTL** — 简单的直接映射逻辑页→物理页转换，GREEDY 垃圾回收。
- **10 个新增配置项** — `hbf_use_phase2`、`hbf_num_subarrays`、`hbf_max_active`、`hbf_tR`、`hbf_tPROG`、`hbf_tBERS`、`hbf_page_size`、`hbf_pages_per_block`、`hbf_mshr_enabled`、`hbf_ftl_enabled`。

### v0.1 — 固定延迟 HBF

与 DRAM 并列的基础内存层：

- 通过 `memory_config::is_hbf_addr()` 实现地址范围路由
- 固定延迟 FIFO（`hbf_ctrl_t`），由 `hbf_latency` 配置
- `hbf_route_all` 测试模式，强制所有 L2 miss 走 HBF
- 每个 memory partition 独立统计读/写/延迟/队列深度

## 快速开始

环境要求：Ubuntu 20.04+，CUDA 11–12，NVIDIA GPU。

```bash
# 编译
export CUDA_INSTALL_PATH=/usr/local/cuda
source ./gpu-simulator/setup_environment.sh
bash setup_hbf.sh                    # 将 HBF 补丁打入 gpgpu-sim
make -j$(nproc) -C ./gpu-simulator

# 基线测试（仅 DRAM，PTX 模式）
bash run_smoke_test.sh

# HBF 测试（PTX 模式）
bash run_hbf_test.sh

# Trace-driven 测试（需预先下载 trace）
./util/tracer_nvbit/install_nvbit.sh  # 一次性
make -C ./util/tracer_nvbit/
# 然后在真实 GPU 上生成 trace，用 accel-sim.out 回放
```

HBF 开箱即用的配置文件位于 `hbf/gpgpusim_hbf.config`。Phase 2 关键选项：

```
-gpgpu_hbf_enabled 1
-gpgpu_hbf_use_phase2 1
-gpgpu_hbf_num_subarrays 16384
-gpgpu_hbf_max_active 64
-gpgpu_hbf_tR 15000
-gpgpu_hbf_mshr_enabled 1
```

## 参考文献

- [Accel-Sim](https://github.com/accel-sim/accel-sim-framework) — GPU 仿真框架（ISCA 2020），本项目基于此构建
- [GPGPU-Sim](https://github.com/gpgpu-sim/gpgpu-sim_distribution) — Accel-Sim 使用的时序模型核心
- [MQSim](https://github.com/CMU-SAFARI/MQSim) — SSD 模拟器（FAST 2018），NAND 时序参数参考来源
- [CXL-MQSim](https://github.com/sang-jun-kim/CXL-MQSim) — CXL 闪存扩展模拟器，在概念上与本项目最接近
- [H3](https://doi.org/10.1109/lca.2026.3660969) — Hybrid Architecture Using HBM and HBF for Cost-Efficient LLM Inference（SK hynix, IEEE CAL 2026）
