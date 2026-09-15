#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
GSIM="$ROOT/gpu-simulator/gpgpu-sim"
CUDA_ROOT="${CUDA_INSTALL_PATH:-/usr/local/cuda}"
shopt -s nullglob
libs=("$GSIM"/lib/gcc-*/cuda-*/release/libcudart.so)
[[ ${#libs[@]} == 1 ]] || { echo "Expected one simulator libcudart.so; run make build" >&2; exit 1; }
LIB_DIR="$(dirname "${libs[0]}")"
"${CXX:-g++}" -O2 -std=c++17 -I"$CUDA_ROOT/include" -I"$GSIM/src" -I"$GSIM/libcuda" \
  "$ROOT/experiments/25_t02_t03_comparison/replay.cc" -L"$LIB_DIR" \
  -Wl,-rpath,"$LIB_DIR" -lcudart -lz -pthread -o "$ROOT/experiments/25_t02_t03_comparison/replay"
