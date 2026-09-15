# HBF-Sim

HBF-Sim is a specification-driven GPU/high-bandwidth-flash simulator integrated
with Accel-Sim/GPGPU-Sim. It models page-granular request coalescing, channel
and subarray affinity, page buffers, logic-die caching, full-page writes,
controller scheduling, finite admission, and request/completion tracing.

The public repository contains the simulator, pinned integration inputs, trace
inputs, scripts, and the exact numerical summaries used in the paper.
Manuscript sources and `docs/` are maintained separately and are intentionally
excluded from this repository. Generated run directories are never committed.

## Quick start

Linux, Bash, Git, GNU Make, CMake, a C++ compiler, Python 3.10+, and CUDA 11/12
with `nvcc` are required. A physical NVIDIA GPU is not required for simulator
runs.

```bash
python3 -m venv .venv && source .venv/bin/activate
python -m pip install -r requirements.txt
export CUDA_INSTALL_PATH=/usr/local/cuda
make bootstrap
make build JOBS="$(nproc)"
make test
make artifact-check
```

`make bootstrap` checks out the exact upstream revision in
`artifact/integration.env`, installs the active HBF patch and canonical source
set, and verifies generated parser snapshots. Existing mismatched checkouts
are rejected rather than overwritten.

## Reproduce the paper evidence

First inspect the complete suite plan:

```bash
make paper-plan
```

Use a new tag for every run. The default `core` suite covers functional
validation, the external trace smoke, MSHR/MQSim comparisons, media scaling, placement and hotspot
mapping, read/write isolation, and host/capacity checks:

```bash
make setup-mqsim
RUN_TAG=paper-001 make reproduce-paper
```

Run one suite with `SUITE=validation|trace|media|placement|hotspots|isolation|cost|comparison`.
The comparison suite requires `make setup-mqsim`; it pins MQSim to the recorded
revision. The optional full Qwen3-1.7B replay requires the verified trace bundle:

```bash
make prepare-qwen
SUITE=qwen RUN_TAG=qwen-001 make reproduce-paper
```

Every run records commands, configuration and binary hashes, complete logs,
trace-validation reports, and return codes. Existing tags are immutable.

## Released evidence inputs

`artifact/reference/` contains frozen CSV summaries for validation, media
scaling, placement, MSHR feedback, MQSim comparison, read/write boundaries,
and host/capacity measurements. `artifact/inputs/lud/` contains the small
Accel-Sim LUD trace used by `make trace-smoke`. `artifact/inputs/qwen3/` contains
three verified archive parts for the 1,819-kernel Qwen3-1.7B remapped trace;
`make prepare-qwen` verifies every part and member hash before unpacking.
Model weights and native GPU capture data are not distributed.

See [evaluation instructions](artifact/ARTIFACT_EVALUATION.md) and
[release verification](artifact/VERIFICATION.md) for measurement boundaries and
the checks performed on a clean source export.

## Repository map

- `hbf/`: maintained HBF model and unit tests.
- `gpu-simulator/`: Accel-Sim/GPGPU-Sim integration tree.
- `artifact/`: pinned revisions, configs, parser snapshots, references, and trace inputs.
- `experiments/`: curated reproducibility runners; abandoned development suites are local-only.
- `util/traces/`: exact `hbf-trace-v1` validation and remapping tools.
- `scripts/`: bootstrap, build, test, packaging, and reproduction helpers.

Figures can be regenerated from released CSVs with `make figures`. The model is
component-validated; timings and bandwidth parameters are research assumptions,
not silicon cycle-accuracy claims.

## Upstream

- [Accel-Sim](https://github.com/accel-sim/accel-sim-framework)
- [GPGPU-Sim](https://github.com/accel-sim/gpgpu-sim_distribution)
- [OCP HBF specification](https://www.opencompute.org/documents/ocp-hbf-architecture-specification-v0-7-0-final-pdf)
