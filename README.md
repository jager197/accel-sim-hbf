# HBF-Sim

An HBF (High Bandwidth Flash) simulator built on top of Accel-Sim/GPGPU-Sim.

## Background

HBF is a new flash memory technology being developed by SanDisk and SK hynix. The idea is to stack 3D NAND dies on top of a logic die using TSVs, then place the whole thing directly on the GPU interposer alongside HBM. Unlike traditional SSDs, HBF splits the NAND array into thousands of independently-addressable micro sub-arrays. The logic die orchestrates parallel access across all of them, achieving aggregate bandwidth close to HBM with 8–16× the capacity at much lower cost.

HBF is still in early stages — first chip samples are expected in 2H 2026, with commercial products in 2027. A few academic groups are working on related research (e.g., HAVEN), but they all use internal simulators. Nothing is open-source.

I'm a first-year PhD student and my research involves HBF-integrated GPU architectures. Since there's no publicly available simulation platform, I decided to build one myself.

## What's Done

A fixed-latency HBF controller that can run basic memory accesses:

- Address-range partitioning: requests above a configurable base address are routed to HBF, everything else goes to DRAM
- HBF requests enter a fixed-latency FIFO and return after `hbf_latency` cycles
- Configurable: HBF capacity, latency, max outstanding requests
- Statistics: read/write counts, average latency, queue depth per partition

## Planned

- Cycle-accurate NAND sub-array state machines (read/program/erase timing)
- Logic die parallel scheduler (how many sub-arrays can be active simultaneously)
- MSHR coalescing (merge multiple 64B cache-line requests into one NAND page read)
- Simple page-level FTL with garbage collection
- Write buffer

## How to Run

Prerequisites: Ubuntu 20.04+, CUDA 11–12, an NVIDIA GPU.

```bash
# Build
export CUDA_INSTALL_PATH=/usr/local/cuda
source ./gpu-simulator/setup_environment.sh
make -j$(nproc) -C ./gpu-simulator

# Baseline test (DRAM only)
bash run_smoke_test.sh

# HBF test
bash run_hbf_test.sh
```

HBF is configured by adding these options to `gpgpusim.config`:

```
-gpgpu_hbf_enabled 1
-gpgpu_hbf_base_addr 274877906944
-gpgpu_hbf_size 549755813888
-gpgpu_hbf_latency 10000
-gpgpu_hbf_max_outstanding 256
```

A ready-to-use HBF config is at `gpu-simulator/gpgpu-sim/configs/tested-cfgs/SM7_QV100/gpgpusim_hbf.config`.

## References

- [Accel-Sim](https://github.com/accel-sim/accel-sim-framework) — GPU simulation framework (ISCA 2020), this project is based on it
- [GPGPU-Sim](https://github.com/gpgpu-sim/gpgpu-sim_distribution) — the timing model core used by Accel-Sim
- [MQSim](https://github.com/CMU-SAFARI/MQSim) — SSD simulator (FAST 2018), NAND timing parameters are referenced from here
- [CXL-MQSim](https://github.com/sang-jun-kim/CXL-MQSim) — CXL flash expander simulator, closest in concept to what we're building
