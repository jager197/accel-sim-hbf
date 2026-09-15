# Release verification (2026-09-15)

Verification uses an exported public source tree containing neither `paper/`
nor `docs/`. GPGPU-Sim is installed from its pinned upstream revision with the
published integration patch and canonical HBF files.

- Clean bootstrap and full simulator build passed with CUDA 12.9 / GCC 13.3.0.
- Installation verification passed: 18 integration paths, 16 HBF files,
  25 generated parser snapshots, and 45 HBF configuration registrations.
- Ten Python tool regression tests and all three C++ model test executables
  passed. Release-source syntax, portable configuration generation, immutable
  input hashes, and figure generation passed. A shell regression verifies that
  cumulative GPU cycles are read from the complete log even when HBF statistics
  appear after them; HBF per-stack aggregation remains separate.
- Functional routing/read tests and the 158-pair LUD trace passed. Demand smoke
  reproduces 5,508 cycles. Aggregation smoke completes in 5,520 cycles with
  the current implementation versus 5,517 in the historical reference; all
  32 requests, one page read and 31 merges remain conserved. Historical
  references have not been rewritten to hide this small timing difference.
- MSHR/MQSim comparison passed: 24 complete GPU cases, 24 exact controller-event
  identity replays, 24 counterfactual transfers and eight external pairs.
  The dense 15-us/128-slot point reproduces 61.1176% closed-loop and 0.8164%
  frozen-ingress benefits.
- All nine media-scaling and 24 placement cells match the checked reference
  cycles, service counts and latency/occupancy fields. All 36 hotspot cells
  match their reference cycles, page services, array reads and total read p95.
- All 37 isolation/deadline outcomes passed collection and matched reference
  cycles, page services, array reads and read/write p95, including the one
  explicitly infeasible assembly-deadline configuration.
- Cost/capacity rerun passed all ten alternating idle-path measurements
  (181,707 cumulative GPU cycles each) and the sparse 512-GiB test
  (20 conserved sector requests, zero mapping/capacity errors and metadata
  allocation). The completed rerun uses the corrected global-cycle parser.
- All 1,819 Qwen input files passed archive/member hash verification. A fresh
  real kernel with HBF weight accesses completed with 128 read requests and
  completions, one array read and 4,096 returned bytes. The full historical
  log was independently parsed for the released aggregate reference.

The full Qwen sequence (historically about 18 host hours) was not rerun during
release preparation. The original full log lacks a retained `run.rc`; its
reference metadata therefore identifies aggregate-log validation explicitly.
Fresh Qwen runs require a zero exit code, every listed kernel completion,
consistent final counters, and read-only 32-byte sector traffic.

Host time/RSS are host-dependent measurements, not exact regression targets.
The Docker image and fresh native GPU capture were not exercised during this
release check; simulator compilation and bundled-input replay were exercised.
