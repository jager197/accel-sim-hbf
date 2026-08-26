# HBF-Sim

基于 Accel-Sim/GPGPU-Sim 构建的 HBF（High Bandwidth Flash）模拟器。

## 背景

HBF 是 SanDisk 和 SK hynix 正在联合研发的新型闪存技术。其思路是将 3D NAND 芯片通过 TSV 堆叠在逻辑芯片（logic die）之上，然后将整个封装直接放置在 GPU interposer 上与 HBM 并列。与传统 SSD 不同，HBF 把 NAND 阵列拆分为数千个可独立寻址的微子阵列（micro sub-array），由 logic die 统一调度并行访问，聚合带宽接近 HBM，容量是 HBM 的 8–16 倍，成本仅为 HBM 的 1/8。

HBF 目前仍在早期阶段——首批样片预计 2026 下半年出货，商用产品预计 2027 年。少数学术团队在做相关研究（如 HAVEN），但均使用内部模拟器，尚无开源方案。

本人的研究涉及 HBF 集成的 GPU 架构。因缺乏公开的仿真平台，于是自行构建。

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
