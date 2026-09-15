# Curated experiment suite

This directory publishes only the runners and workloads needed by the current
paper. Numeric directory names are retained for provenance; gaps correspond to
excluded development studies. `13_rq2_locality/` supplies only the shared CUDA
locality workload, not the superseded RQ2 matrix.

See [artifact evaluation](../artifact/ARTIFACT_EVALUATION.md) for the complete
paper-to-runner mapping and expected results.

```bash
make paper-plan
make setup-mqsim
SUITE=core RUN_TAG=core-001 MAX_PAR=1 make reproduce-paper
make prepare-qwen
SUITE=qwen RUN_TAG=qwen-001 make reproduce-paper
```

All runs use fresh output tags. Individual suites are `validation`, `trace`,
`comparison`, `media`, `placement`, `hotspots`, `isolation`, `cost`, and `qwen`.
`core` selects all except Qwen; `all` includes it. Generated results, model
weights, build products and intermediate capture files stay outside Git.
