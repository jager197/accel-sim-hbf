#!/usr/bin/env bash
# Remap one Accel-Sim trace and exercise the complete GPU-to-HBF path.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
# shellcheck disable=SC1091
source "$ROOT/scripts/artifact_lib.sh"
# shellcheck disable=SC1091
source "$HERE/../common/env.sh"
# shellcheck disable=SC1091
source "$ROOT/artifact/integration.env"

PYTHON="${PYTHON:-python3}"
TAG="${RUN_TAG:-$(date -u '+%Y%m%d-%H%M%S')}"
OUT="$HERE/results/$TAG"
TRACE_SRC="${TRACE_SRC:-$ROOT/artifact/inputs/lud}"
KERNEL="${TRACE_KERNEL:-kernel-8.traceg}"
BASE_CONFIG="$GPGPUSIM_ROOT/configs/tested-cfgs/SM7_QV100/gpgpusim_hbf.config"
TRACE_CONFIG="$ROOT/gpu-simulator/configs/tested-cfgs/SM7_QV100/trace.config"
SIMULATOR="$ROOT/gpu-simulator/bin/release/accel-sim.out"
VALIDATOR="$ROOT/util/traces/validate_hbf_trace.py"

validate_run_tag "$TAG"
[[ "$KERNEL" == "$(basename "$KERNEL")" && "$KERNEL" =~ ^[A-Za-z0-9._-]+$ ]] ||
  artifact_die "TRACE_KERNEL must be a safe basename: $KERNEL"
require_file "$TRACE_SRC/$KERNEL"
require_file "$TRACE_SRC/kernelslist.g"
require_file "$BASE_CONFIG"
require_file "$TRACE_CONFIG"
require_executable "$SIMULATOR"
prepare_fresh_output "$OUT"
mkdir "$OUT/traces" "$OUT/configs"

"$PYTHON" "$ROOT/util/traces/remap_global_addresses.py" \
  --input "$TRACE_SRC/$KERNEL" \
  --output "$OUT/traces/$KERNEL" \
  --base "$HBF_BASE" --span 549755813888 --wrap \
  --manifest "$OUT/traces/$KERNEL.manifest.json" > "$OUT/remap.json"
require_file "$OUT/traces/$KERNEL.manifest.json"

memcpy_line="$(awk '/^MemcpyHtoD,/{print; exit}' "$TRACE_SRC/kernelslist.g")"
[[ -n "$memcpy_line" ]] || memcpy_line='MemcpyHtoD,0x0000000000000000,0'
printf '%s\n%s\n' "$memcpy_line" "$KERNEL" > "$OUT/traces/kernelslist.g"

config="$OUT/configs/trace.config"
sed \
  -e 's/^-gpgpu_hbf_l2_policy .*/-gpgpu_hbf_l2_policy 0/' \
  -e 's/^-gpgpu_hbf_num_stacks .*/-gpgpu_hbf_num_stacks 1/' \
  -e 's/^-gpgpu_hbf_stack_map .*/-gpgpu_hbf_stack_map 0/' \
  -e 's/^-gpgpu_hbf_num_channels .*/-gpgpu_hbf_num_channels 4/' \
  -e 's/^-gpgpu_hbf_num_subarrays .*/-gpgpu_hbf_num_subarrays 32/' \
  -e 's/^-gpgpu_hbf_max_active .*/-gpgpu_hbf_max_active 8/' \
  -e 's/^-gpgpu_hbf_tR .*/-gpgpu_hbf_tR 20/' \
  -e 's/^-gpgpu_hbf_tPROG .*/-gpgpu_hbf_tPROG 40/' \
  -e 's/^-gpgpu_hbf_tBERS .*/-gpgpu_hbf_tBERS 80/' \
  -e 's/^-gpgpu_hbf_write_timeout_policy .*/-gpgpu_hbf_write_timeout_policy 0/' \
  -e 's/^-gpgpu_hbf_trace_level .*/-gpgpu_hbf_trace_level 1/' \
  -e "s|^-gpgpu_hbf_trace_file .*|-gpgpu_hbf_trace_file \"$OUT/run/hbf.csv\"|" \
  "$BASE_CONFIG" > "$config"
cat "$TRACE_CONFIG" >> "$config"
grep -Fxq -- '-gpgpu_hbf_num_stacks 1' "$config"
grep -Fxq -- '-gpgpu_hbf_num_channels 4' "$config"
grep -Fxq -- '-gpgpu_hbf_write_timeout_policy 0' "$config"

RUN_TIMEOUT="${RUN_TIMEOUT:-300}" "$HERE/../common/run_sim.sh" \
  "$OUT/run" "$config" "$SIMULATOR" \
  -config "$config" -trace "$OUT/traces/kernelslist.g"
assert_run_success "$OUT/run"

"$PYTHON" "$VALIDATOR" "$OUT/run/hbf.csv" \
  --channels 4 --subarrays 32 --require-nonempty \
  --json "$OUT/run/trace_validation.json" > "$OUT/run/trace_validation.txt"
validate_trace_report "$OUT/run/trace_validation.json"
cycles="$(last_stat "$OUT/run/run.log" 'gpu_sim_cycle =')"
requests="$(sum_stat "$OUT/run/run.log" 'HBF Total Requests:')"
require_positive_value cycles "$cycles"
require_positive_value hbf_requests "$requests"

PROVENANCE_COMMAND="RUN_TAG=$TAG make trace-smoke" \
  "$ROOT/scripts/record_provenance.sh" "$OUT" \
  "$ROOT/Makefile" "$ROOT/artifact/integration.env" \
  "$ROOT/artifact/hbf_sources.txt" "$ROOT/$GPGPUSIM_PATCH" \
  "$HERE/run.sh" "$ROOT/util/traces/remap_global_addresses.py" "$VALIDATOR" \
  "$TRACE_SRC/$KERNEL" "$TRACE_SRC/kernelslist.g"

cat > "$OUT/README.txt" <<EOF
HBF-Sim trace-driven smoke bundle
UTC tag: $TAG
source_trace=$TRACE_SRC/$KERNEL
remap_manifest=$OUT/traces/$KERNEL.manifest.json
The run produced positive HBF traffic and passed exact hbf-trace-v1 validation.
This wiring smoke explicitly uses partial-write compatibility because the
unmodified LUD kernel emits sub-page stores. Strict full-page writes are
covered separately by RQ3 and remain the installed default.
EOF
echo "trace smoke bundle: $OUT"
