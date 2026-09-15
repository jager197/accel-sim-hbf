# System-boundary experiments

This suite tests admission-window metric reversal and the arrival-phase boundary
between read-priority scheduling and resource isolation. It uses passive request diagnostics and controlled workload timing.

## Measurement

Set `HBF_DIAGNOSTICS=1` with an enabled HBF trace. The controller writes an
additional `hbf.csv.diagnostics.csv`; the original `hbf-trace-v1` schema and
simulated event timing are unchanged. Diagnostic records include native
mem_fetch ID and creation cycle. INGRESS and COMPLETED are paired by request ID.
Creation-to-admission, admission-to-controller-return, and their sum are recorded.
The last interval still excludes final GPU response consumption.

The controller also reports queued read-request ticks and the subset for which
the target subarray is PROGRAMMING. These are aggregate interference exposure,
not additive wall-clock critical-path times. Counters are active only with the
optional diagnostic output. Observation on/off equivalence is tested using
identical original trace CSVs and kernel cycles.

## Run

```bash
python3 experiments/17_system_boundaries/run.py --tag NEW_TAG --jobs 4
```

The default run records 89 necessary outcomes: 52 read configurations and 37 phase/deadline controls. One diagnosed phase cell is retained as deadline-infeasible with no performance metrics; other failures still stop the study.

All tags must be new. `--cases` selects explicitly named cells and saves the
selected plan; `--pilot` selects a bounded diagnostic subset. Failed or incomplete
bundles are not complete matrix evidence. `collect.py` assembles each declared
family only when every expected case has exactly one successful source and raw
logs/traces rederive the recorded metrics.

The native mem_fetch creation timestamp is 32-bit; runs exceeding that range
are rejected by analysis. No statistical independence of individual sectors is
assumed. All comparisons are deterministic configuration-specific effects.

## Timing and workload validity

The final mixed workload uses inline PTX `mov.u32 %clock`, because CUDA 12.9
lowers the normal clock builtins to `%clock64`, whose upstream simulator
emulation scales the counter by four. One 1024-thread CTA supplies a complete
4 KiB page. The fixed 17-CTA grid includes one reader CTA and up to 16 writer
CTAs. Writes to incomplete pages still fail under strict policy.

The phase pilots exposed premature idle draining: the old code aged a page
from its first fragment while fragments were still arriving. Idle draining now
requires 100 DRAM ticks without any new buffered-write fragment. The explicit
page-assembly deadline remains measured from the first fragment. This timing
fix is recorded separately from passive diagnostics.

The initial exploration and corrected confirmatory phase study use separately
identified binaries and source snapshots. Never pool different versions as
replicates or relabel the initial nominal delay as its intended core-cycle time.

## Released evidence

The public runner takes portable templates from `artifact/configs/`; it has no
dependency on experiment 16 or historical result directories. The release
harness selects the 37 phase/deadline controls for `SUITE=isolation` and the
24 MSHR feedback configurations for `SUITE=comparison`. Read-only exploration
cells outside that selection are available via explicit `--cases` only.
Frozen phase outcomes are in `artifact/reference/phase_boundary_summary.csv`.
