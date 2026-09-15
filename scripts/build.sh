#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"
export CUDA_INSTALL_PATH="${CUDA_INSTALL_PATH:-/usr/local/cuda}"

[[ "$JOBS" =~ ^[1-9][0-9]*$ ]] || {
  echo "build: JOBS must be a positive integer" >&2
  exit 2
}
[[ -x "$CUDA_INSTALL_PATH/bin/nvcc" ]] || {
  echo "build: nvcc not found at $CUDA_INSTALL_PATH/bin/nvcc" >&2
  exit 1
}
command -v make >/dev/null 2>&1 || {
  echo "build: make is required" >&2
  exit 1
}

"$ROOT/scripts/ensure_pybind11.sh"
"$ROOT/scripts/verify_install.sh"
# The upstream setup scripts predate `set -u` and intentionally inspect unset
# environment variables. Disable nounset only while sourcing them.
set +u
# shellcheck disable=SC1091
source "$ROOT/gpu-simulator/setup_environment.sh"
set -u
make -j"$JOBS" -C "$ROOT/gpu-simulator"

binary="$ROOT/gpu-simulator/bin/${ACCELSIM_CONFIG:-release}/accel-sim.out"
[[ -x "$binary" ]] || {
  echo "build: expected simulator was not produced: $binary" >&2
  exit 1
}
echo "build: $binary"
