# MSHR feedback and external page-service comparison

```bash
make setup-mqsim
SUITE=comparison RUN_TAG=comparison-001 make reproduce-paper
```

Outputs are under `results/comparison-001/`; the 24 full GPU runs are under
`../17_system_boundaries/results/comparison-001/`. Reusing a tag is an error.

`build.sh` links the replay driver against the same installed C++ HBF library
as full GPU simulation. `run_replay.py` first reproduces all 24 controller event
traces exactly, then executes 24 opposite-MSHR frozen-ingress counterfactuals.
`run_external.py` runs eight paired HBF/MQSim page-service workloads. `analyze.py`
checks request multisets, completions and independent read-timing slopes.

Full/replay duration is first INGRESS to last COMPLETED, not GPU kernel time
for device replay. External input is 64 full 4-KiB pages, represented as one
8-sector NVMe request per page in MQSim and 32 merged 128-B fragments in HBF.
The nominal 192-GB/s channel rate is synthetic; native NVMe/ONFI and HBF
bandwidth-credit interfaces remain different. These are model comparisons,
not silicon calibration.

Frozen numeric references are in `../../artifact/reference/`. Use
`figures/gen_fig_comparison.py --data-dir PATH --output-dir PATH` for either
fresh or released reference CSVs.
