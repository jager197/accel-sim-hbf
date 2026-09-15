# Qwen3-1.7B full decode

```bash
make prepare-qwen
SUITE=qwen RUN_TAG=qwen-001 make reproduce-paper
```

The verified bundled trace preserves all 1,819 kernels of one BF16 decode
forward pass, all 28 layers, and weight-only HBF remapping. The replay runner
uses `artifact/configs/qwen3.config`, preserving the measured L1/L2 bypass
settings. Fresh outputs and aggregate validation go to `results/qwen-001/`.

For a new GPU capture, optional requirements, exact model revision, shard
hashes, and download/native/capture/remapping scripts are included here. See
[artifact evaluation](../../artifact/ARTIFACT_EVALUATION.md) for the original
GPU software environment, capture commands and validation boundaries.
