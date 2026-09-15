# HBF-Sim

[English](README.md) | [简体中文](README_CN.md)

HBF-Sim is a configurable GPU–high-bandwidth-flash simulator built on
Accel-Sim/GPGPU-Sim. It connects GPU execution to NAND page service, making it
possible to study how memory placement, request coalescing, controller scheduling,
and flash parallelism affect GPU performance.

## Features

- **GPU–HBF feedback:** execution-driven CUDA workloads and trace-driven replay,
  with admission pressure and completions feeding back into GPU execution.
- **Configurable topology:** independent HBF stacks, channels, channel-affine
  subarrays, and limits on concurrent media operations.
- **Page-granular access:** page-keyed MSHR coalescing, NAND page buffers,
  logic-die caching, and full-page write aggregation.
- **Policy exploration:** address mapping, demand/aggregation reads, and FCFS,
  read-priority, and bounded write-drain scheduling.
- **Request tracing:** request lifecycle events and validation tools for
  analyzing latency, traffic, and completion ordering.

The model follows the OCP HBF interface specification. NAND timings and link
parameters are configurable; model validation and supported boundaries are
described in the [validation notes](artifact/VERIFICATION.md).

## Getting started

Requirements: Linux, Bash, Git, GNU Make, CMake, a C++ compiler, zlib development
headers, `makedepend` (`xutils-dev`), Python 3.10+, and CUDA 11/12 with `nvcc`.
Simulator execution does not require a physical NVIDIA GPU; capturing new
application traces does.

```bash
git clone --branch dev https://github.com/jager197/accel-sim-hbf.git
cd accel-sim-hbf

python3 -m venv .venv
source .venv/bin/activate
python -m pip install -r requirements.txt
export CUDA_INSTALL_PATH=/usr/local/cuda

make bootstrap
make build JOBS=4
make test
```

`make bootstrap` installs the HBF extension on the pinned GPGPU-Sim revision.
See the [installation guide](artifact/README.md) for dependency details and the
CUDA development container.

## Usage

Run the included functional examples:

```bash
make reproduce-smoke RUN_TAG=smoke-001
```

This checks a DRAM numerical baseline, HBF channel routing, and same-page read
coalescing. To run the bundled LUD trace through the GPU–HBF path:

```bash
make trace-smoke RUN_TAG=trace-001
```

Each example writes its configuration, log, and request trace under its
`experiments/` result directory. Choose a new `RUN_TAG` for each execution.

The [HBF configuration](hbf/gpgpusim_hbf.config) documents topology, media
timing, cache, mapping, and scheduler options. Additional workloads and analysis
commands are available in the [experiment guide](experiments/README.md).
Run `make help` to list the main entry points.

## Repository layout

| Directory | Contents |
|---|---|
| `hbf/` | HBF model, configuration, integration patch, and unit tests |
| `gpu-simulator/` | Accel-Sim frontend and GPGPU-Sim integration |
| `experiments/` | Example workloads and experiment runners |
| `artifact/` | Build inputs, sample traces, reference data, and guides |
| `util/traces/` | Trace validation and address remapping |
| `scripts/` | Installation, build, and verification helpers |

## Documentation

- [Installation and build](artifact/README.md)
- [Experiments and workloads](experiments/README.md)
- [Detailed evaluation and Qwen capture/replay](artifact/ARTIFACT_EVALUATION.md)
- [Validation notes](artifact/VERIFICATION.md)

## Acknowledgments and license

HBF-Sim builds on [Accel-Sim](https://github.com/accel-sim/accel-sim-framework)
and [GPGPU-Sim](https://github.com/accel-sim/gpgpu-sim_distribution), with HBF
modeling guided by the [OCP HBF specification](https://www.opencompute.org/documents/ocp-hbf-architecture-specification-v0-7-0-final-pdf).
See [LICENSE](LICENSE) for the repository license and retained copyright notices.
