#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
SANITIZE="${SANITIZE:-none}"
BUILD_DIR="${TMPDIR:-/tmp}/hbf-core-tests-${SANITIZE}"
CXX="${CXX:-g++}"
CUDA_ROOT="${CUDA_INSTALL_PATH:-/usr/local/cuda}"
GPGPU_ROOT="$ROOT/gpu-simulator/gpgpu-sim"

case "$SANITIZE" in
  none) SANITIZER_FLAGS=() ;;
  address|undefined)
    SANITIZER_FLAGS=(-fsanitize="$SANITIZE" -fno-omit-frame-pointer)
    ;;
  *)
    echo "run_tests: SANITIZE must be none, address, or undefined" >&2
    exit 2
    ;;
esac

mkdir -p "$BUILD_DIR"

COMMON_FLAGS=(
  -std=c++17
  -O0
  -g
  -Wall
  -Wextra
  -Wno-unused-parameter
  -I"$CUDA_ROOT/include"
  -I"$ROOT/hbf"
  -I"$GPGPU_ROOT/src/gpgpu-sim"
  -I"$GPGPU_ROOT/src"
  -I"$GPGPU_ROOT/libcuda"
  "${SANITIZER_FLAGS[@]}"
)

"$CXX" "${COMMON_FLAGS[@]}" \
  "$ROOT/hbf/tests/test_p0_core.cc" \
  "$ROOT/hbf/hbf_ftl.cc" \
  "$ROOT/hbf/hbf_subarray.cc" \
  -o "$BUILD_DIR/test_p0_core"

"$CXX" "${COMMON_FLAGS[@]}" \
  "$ROOT/hbf/tests/test_write_coverage.cc" \
  -o "$BUILD_DIR/test_write_coverage"

"$CXX" "${COMMON_FLAGS[@]}" \
  "$ROOT/hbf/tests/test_multistack_mapping.cc" \
  -o "$BUILD_DIR/test_multistack_mapping"

"$BUILD_DIR/test_p0_core"
"$BUILD_DIR/test_write_coverage"
"$BUILD_DIR/test_multistack_mapping"
echo "hbf core tests: passed (sanitize=$SANITIZE)"
