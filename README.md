# HBF-Sim

An HBF (High Bandwidth Flash) simulator built on top of Accel-Sim/GPGPU-Sim.

## Background

HBF is a new flash memory technology being developed by SanDisk and SK hynix. The idea is to stack 3D NAND dies on top of a logic die using TSVs, then place the whole thing directly on the GPU interposer alongside HBM. Unlike traditional SSDs, HBF splits the NAND array into thousands of independently-addressable micro sub-arrays. The logic die orchestrates parallel access across all of them, achieving aggregate bandwidth close to HBM with 8–16× the capacity at much lower cost.

HBF is still in early stages — first chip samples are expected in 2H 2026, with commercial products in 2027. A few academic groups are working on related research (e.g., HAVEN), but they all use internal simulators. Nothing is open-source.

My research involves HBF-integrated GPU architectures. Since there's no publicly available simulation platform, I decided to build one myself.

## Versions

### v0.2 (current) — Cycle-Accurate NAND Controller

Built on v0.1, replaces the fixed-latency FIFO with a real flash controller:

- **NAND sub-array state machines** — IDLE → READING → PROGRAMMING → ERASING, each with configurable per-operation latency (tR / tPROG / tBERS). Follows the `bank_t` pattern from GPGPU-Sim's DRAM model.
- **MSHR coalescing** — multiple 64B cache-line requests to the same 4KB NAND page are merged into a single page read. Verified: 75% coalescing rate on a rodinia trace (16 requests → 4 page reads).
- **Power-limited parallelism** — configurable max simultaneous sub-array operations (`hbf_max_active`).
- **Page-level FTL** — simple direct-mapped logical→physical translation with GREEDY garbage collection.
- **10 new config options** — `hbf_use_phase2`, `hbf_num_subarrays`, `hbf_max_active`, `hbf_tR`, `hbf_tPROG`, `hbf_tBERS`, `hbf_page_size`, `hbf_pages_per_block`, `hbf_mshr_enabled`, `hbf_ftl_enabled`.

### v0.1 — Fixed-Latency HBF

Basic memory tier alongside DRAM:

- Address-range partitioning via `is_hbf_addr()` in `memory_config`
- Fixed-latency FIFO (`hbf_ctrl_t`), configured by `hbf_latency`
- `hbf_route_all` test mode for forcing all L2 misses through HBF
- Read/write/latency/queue depth statistics per partition

## How to Run

Prerequisites: Ubuntu 20.04+, CUDA 11–12, an NVIDIA GPU.

```bash
# Build
export CUDA_INSTALL_PATH=/usr/local/cuda
source ./gpu-simulator/setup_environment.sh
bash setup_hbf.sh                    # apply HBF patches to gpgpu-sim
make -j$(nproc) -C ./gpu-simulator

# Baseline test (DRAM only, PTX mode)
bash run_smoke_test.sh

# HBF test (PTX mode)
bash run_hbf_test.sh

# Trace-driven test (requires pre-downloaded traces)
./util/tracer_nvbit/install_nvbit.sh  # one-time
make -C ./util/tracer_nvbit/
# Then generate traces on real GPU and replay with accel-sim.out
```

A ready-to-use HBF config is at `hbf/gpgpusim_hbf.config`. Key Phase 2 options:

```
-gpgpu_hbf_enabled 1
-gpgpu_hbf_use_phase2 1
-gpgpu_hbf_num_subarrays 16384
-gpgpu_hbf_max_active 64
-gpgpu_hbf_tR 15000
-gpgpu_hbf_mshr_enabled 1
```

## References

- [Accel-Sim](https://github.com/accel-sim/accel-sim-framework) — GPU simulation framework (ISCA 2020), this project is based on it
- [GPGPU-Sim](https://github.com/gpgpu-sim/gpgpu-sim_distribution) — the timing model core used by Accel-Sim
- [MQSim](https://github.com/CMU-SAFARI/MQSim) — SSD simulator (FAST 2018), NAND timing parameters are referenced from here
- [CXL-MQSim](https://github.com/sang-jun-kim/CXL-MQSim) — CXL flash expander simulator, closest in concept to what we're building
