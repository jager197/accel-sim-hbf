# Artifact evaluation

## Installation and checks

Start with the root README installation commands. `make test-tools` runs the
Python trace/remapping regressions, release-input checks, numerical plot
regeneration, and runner failure/overwrite checks. `make test` additionally
builds and runs the C++ model tests. `make artifact-check` verifies the installed
patch, canonical HBF sources, config registrations, and generated parsers.

The `paper/` and `docs/` directories are not part of this artifact. None of the
commands below requires those directories or a previous experiment run.

## Coverage and commands

Each tag must be new. `MAX_PAR` is the number of concurrent simulation cases
within a suite (1–4); suites execute sequentially. Failed suites stop the run and
retain their logs under `artifact_runs/reproduction/<tag>/`.

| Paper evidence | Suite | Published runner / input | Acceptance |
|---|---|---|---|
| §5.1 functional validation | `validation` | `10_validation/run.sh` | numerical DRAM baseline; 64 requests at 1/2/4/8/16 channels; 32 same-page requests, one page read, 31 MSHR hits |
| §5.1 LUD integration | `trace` | `11_trace_smoke/run.sh`, `artifact/inputs/lud/` | 158 request/completion pairs; exact lifecycle schema |
| §5.2 MSHR feedback | `comparison` | `25_t02_t03_comparison/`, boundary workload | 24 GPU runs, 24 exact controller-event identity replays, 24 opposite-MSHR transfers |
| §5.3 external page service | `comparison` | same driver, pinned MQSim | 8 pairs; 64 full pages/256 KiB each; service timing slope |
| §5.4 media resource scaling | `media` | `19_media_saturation/run.py` | 9 runs; channel, supply, stream-length and fixed-slot controls |
| §5.5 Qwen decode | `qwen` | `23_qwen3_1p7b/`, bundled trace | all 1,819 kernel completions and final aggregate read counters |
| §5.6 cost/capacity | `cost` | `15_cost_capacity/run.sh` | 5 alternating repetitions per mode; cumulative kernel cycles; sparse 512 GiB span |
| §6.1 placement | `placement` | `18_placement_cases/run.py` | 24 cells; observed page ownership and paired lifecycle metrics |
| §6.1 hotspot mapping | `hotspots` | `20_placement_hotspots/run.py` | 36 cells; 64/128 pages, 1/8 entries, strides 1/4/32, three mappings |
| §6.2 isolation/deadlines | `isolation` | `17_system_boundaries/run.py`, `collect.py` | 37 outcomes: 36 completed, one explicit assembly-deadline failure |

```bash
make paper-plan
make setup-mqsim
SUITE=core RUN_TAG=core-001 MAX_PAR=1 make reproduce-paper
make prepare-qwen
SUITE=qwen RUN_TAG=qwen-001 make reproduce-paper
```

`core` includes every suite except the long full-model Qwen replay. `SUITE=all`
includes Qwen as well and requires both MQSim and unpacked Qwen inputs first.
An individual suite can be selected using any name in the table. For example:

```bash
SUITE=media RUN_TAG=media-001 make reproduce-paper
SUITE=isolation RUN_TAG=isolation-001 make reproduce-paper
```

The comparison runner uses a unique output directory per tag and the same
installed C++ HBF library for GPU runs and device replay. MQSim is downloaded
and built by `make setup-mqsim` at commit
`51f0f2d3fed92d88ef4a0fa61a38024b07bf9d16`. An existing modified or differently
versioned MQSim checkout is rejected.

## Numerical reference and figures

`reference/manifest.json` identifies each original result source, its original
hash, and the released reference hash. Host-specific directory prefixes were
removed from provenance columns; numerical columns are unchanged. These small
references support checking and plotting without distributing historical run
trees. Fresh runs generate their own configurations, hashes, logs and traces.

```bash
make figures
```

Plots are written to `artifact_runs/figures/`. To plot fresh comparison results:

```bash
python experiments/25_t02_t03_comparison/figures/gen_fig_comparison.py \
  --data-dir experiments/25_t02_t03_comparison/results/comparison-001 \
  --output-dir artifact_runs/figures/comparison-001
```

The reference MSHR dense 15-us/128-slot point has a 61.118% closed-loop
completion-window reduction and 0.816% frozen-ingress reduction. Media scaling
has about 8.738 to 139.285 GB/s **array** bandwidth. Returned GPU sectors and
useful scalar words have different byte counts. Host wall times and RSS depend
on the host and toolchain; cumulative simulated cycles are the deterministic
workload metric.

For fresh placement/isolation/cost figures, collect the three corresponding
CSVs in a directory named `placement_cases.csv`, `phase_boundary_summary.csv`,
and `cost_overhead_audited.csv`, then pass it as `--data-dir` to
`artifact/figures/gen_case_studies.py`. The fresh cost runner already reports
cumulative cycles in its `cycles` field; the legacy reference also retains the
old final-kernel field for audit history. Media plots accept the fresh
`summary.csv` through `gen_media_saturation.py --data PATH`.

## Qwen input and replay

`make prepare-qwen` joins three archive parts (about 82 MiB in total), verifies
part and member SHA-256 hashes, and extracts 1,819 `.traceg.xz` files to
`artifact_runs/inputs/qwen3/`. It rejects an existing destination. The bundle
contains a full second decode forward pass of Qwen3-1.7B after prefill,
BF16/eager attention, batch one, 29-token context, and all 28 layers. Recorded
parameter allocation ranges alone are mapped into HBF; KV, activations and
scratch stay on the ordinary GPU-memory path. No weights are included.

`artifact/configs/qwen3.config` preserves the measured configuration, including
`gpgpu_gmem_skip_L1D=1` and `gpgpu_hbf_l2_policy=0`. The historical result is
352,434,530 cumulative GPU cycles at 1.410 GHz (249.95 ms), 107,540,992 read
requests/completions, and 3,441,311,744 returned sector bytes. This is a kernel
sequence timing boundary, not end-to-end serving latency. Validation reports
final aggregate counters; it does not certify all 107 million request lifecycles.

The historical replay took about 18 host hours. Its uncompressed lifecycle CSV
is much larger than the compressed input; allow ample disk space. A full replay
is deliberately separate from the quick tests. The simulator needs no physical
GPU when replaying the included input.

For a new native capture, install `wget`, `bzip2`, and `bc`, then use an NVIDIA GPU and the NVBit tracer under
`util/tracer_nvbit/`. The original environment was an RTX 4090 with NVIDIA
PyTorch 25.06 (`torch 2.8.0a0+5228986c39.nv25.06`), Transformers 4.57.6, BF16,
and eager attention. The public model revision is
`70d244cc86ccca08cf5af4e1e306ecf908b1ad5e`. The experiment directory retains
model-file hashes, the native run script, parameter remapper, and the capture
pipeline. Install its optional requirements in that GPU environment, then:

```bash
bash util/tracer_nvbit/install_nvbit.sh
make -C util/tracer_nvbit
python -m pip install -r experiments/23_qwen3_1p7b/requirements.txt
python experiments/23_qwen3_1p7b/start_pipeline.py
```

Capture outputs must not exist already. The pipeline verifies model shards,
compares native and instrumented logits, postprocesses the complete capture,
remaps parameters and replays it. Kernel selection/count can differ with a
different GPU or PyTorch/CUDA build; use the bundled input for the exact
published kernel sequence.

## Measurement contract

All configurations are deterministic; requests are not independent replicates.
Controller-local latency starts at INGRESS. Creation-to-return additionally
includes pre-admission waiting. Neither is the same as whole-kernel execution
time. Strict write studies require full byte coverage before PROGRAM and
completion after PROGRAM. The one declared deadline-infeasible isolation cell
is retained without fabricated performance metrics; other failures abort.

The small LUD integration trace contains partial stores, so that smoke explicitly
uses compatibility writes. Strict full-page write semantics are exercised by
the C++ tests and the isolation study. The Python `util/hbf/device_replay.py`
helper remains for tool regressions; it is not the C++ reference model used by
the MSHR or MQSim experiments.
