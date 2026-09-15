# Reproducible integration

The public artifact has no dependency on the private manuscript or historical
experiment directories. Installation inputs are pinned in `integration.env`
and `dependencies.env`; `hbf_sources.txt` and `generated_sources.txt` declare
all copied model sources and parser snapshots.

Use `make bootstrap`, `make build`, `make test`, and `make artifact-check` from
the repository root. `make bootstrap` rejects an existing GPGPU-Sim checkout at
a different revision or with changes outside the declared installation. A fresh
clone is the recommended starting point. CUDA 11/12, a C++ compiler, CMake,
Make, zlib development headers, `makedepend` (`xutils-dev`), and Python 3.10+
are needed. The Dockerfile provides a CUDA 12 development environment.

The active integration patch is against upstream GPGPU-Sim
`6c3cf4ff32110908386d605a7034fc67666a92de`. Canonical HBF sources and generated
parser snapshots are installed separately. The patch captures the GPU-side
hooks used by the experiments; changing only `hbf/` is insufficient to install
the complete system.

- `configs/`: portable experiment templates extracted from the actual runs.
  Only generated output and channel-map paths were cleared. Their provenance
  and hashes are recorded alongside the templates.
- `inputs/lud/`: the small LUD kernel used by the trace smoke.
- `inputs/qwen3/`: the exact remapped Qwen3-1.7B decode input, with hashes for
  archive parts and every trace file. `make prepare-qwen` verifies and extracts it.
- `reference/`: compact numerical references; no historical logs or manuscript.
- `figures/`: numerical plotting code that works directly on public references.

See [ARTIFACT_EVALUATION.md](ARTIFACT_EVALUATION.md) for experiment coverage,
commands, expected checks, measurement boundaries, and capture instructions.
