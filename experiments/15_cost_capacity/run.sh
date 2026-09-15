#!/usr/bin/env bash
# Host simulation-cost and 512 GiB sparse-capacity validation.
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
CONFIGS="$OUT/configs"
BIN="$OUT/workloads_bin"
BASE_CONFIG="$GPGPUSIM_ROOT/configs/tested-cfgs/SM7_QV100/gpgpusim_hbf.config"
VALIDATOR="$ROOT/util/traces/validate_hbf_trace.py"
REPEATS="${COST_REPEATS:-5}"
ITERATIONS="${COST_ITERATIONS:-32}"
ELEMENTS="${COST_ELEMENTS:-4096}"
CAPACITY_BYTES=549755813888
PAGE_SIZE=4096
STACKS=4
CHANNELS=16
SUBARRAYS=32

validate_run_tag "$TAG"
require_positive_integer COST_REPEATS "$REPEATS"
require_positive_integer COST_ITERATIONS "$ITERATIONS"
require_positive_integer COST_ELEMENTS "$ELEMENTS"
require_file "$BASE_CONFIG"
require_file "$VALIDATOR"
require_executable "$CUDA_INSTALL_PATH/bin/nvcc"
prepare_fresh_output "$OUT"
mkdir "$CONFIGS" "$BIN"

"$CUDA_INSTALL_PATH/bin/nvcc" $SM70 --cudart shared -o "$BIN/overhead_probe" \
  "$HERE/overhead_probe.cu"
"$CUDA_INSTALL_PATH/bin/nvcc" $SM70 --cudart shared -o "$BIN/sparse_capacity_probe" \
  "$HERE/sparse_capacity_probe.cu"
require_executable "$BIN/overhead_probe"
require_executable "$BIN/sparse_capacity_probe"

base_overhead_config="$CONFIGS/overhead-base.config"
sed \
  -e 's/^-gpgpu_n_clusters .*/-gpgpu_n_clusters 8/' \
  -e 's/^-gpgpu_n_mem .*/-gpgpu_n_mem 8/' \
  -e 's/^-gpgpu_deadlock_detect .*/-gpgpu_deadlock_detect 0/' \
  -e 's|^-gpgpu_hbf_trace_file .*|-gpgpu_hbf_trace_file ""|' \
  -e 's/^-gpgpu_hbf_trace_level .*/-gpgpu_hbf_trace_level 0/' \
  "$BASE_CONFIG" > "$base_overhead_config"

for mode in disabled enabled; do
  config="$CONFIGS/overhead-$mode.config"
  enabled=1
  [[ "$mode" == disabled ]] && enabled=0
  sed "s/^-gpgpu_hbf_enabled .*/-gpgpu_hbf_enabled $enabled/" \
    "$base_overhead_config" > "$config"
  grep -Fxq -- "-gpgpu_hbf_enabled $enabled" "$config"
done

read_metric() {
  local path="$1" key="$2"
  awk -F= -v key="$key" '$1 == key {print $2}' "$path"
}

printf 'mode,repeat,iterations,elements,cycles,wall_seconds,user_seconds,system_seconds,max_rss_kib,hbf_requests,run_rc,numerical_valid\n' \
  > "$OUT/overhead_summary.csv"
for ((repeat = 1; repeat <= REPEATS; ++repeat)); do
  modes=(disabled enabled)
  (( repeat % 2 == 0 )) && modes=(enabled disabled)
  for mode in "${modes[@]}"; do
    run_dir="$OUT/overhead_${mode}_${repeat}"
    config="$CONFIGS/overhead-$mode.config"
    "$HERE/../common/run_sim.sh" "$run_dir" "$config" "$BIN/overhead_probe" \
      "$ITERATIONS" "$ELEMENTS"
    assert_run_success "$run_dir" \
      "[OVERHEAD-PROBE] completed: $ITERATIONS iterations x $ELEMENTS elements, 0 errors"
    cycles="$(last_stat "$run_dir/run.log" 'gpu_tot_sim_cycle =')"
    wall="$(read_metric "$run_dir/run.time" wall_seconds)"
    user="$(read_metric "$run_dir/run.time" user_seconds)"
    system="$(read_metric "$run_dir/run.time" system_seconds)"
    rss="$(read_metric "$run_dir/run.time" max_rss_kib)"
    require_positive_value cycles "$cycles"
    require_positive_value max_rss_kib "$rss"
    if [[ "$mode" == enabled ]]; then
      requests="$(sum_stat "$run_dir/run.log" 'HBF Total Requests:')"
      [[ "$requests" == 0 ]] || artifact_die "overhead probe routed unexpected HBF traffic"
    else
      requests=0
      ! grep -Fq '========= HBF System Statistics =========' "$run_dir/run.log" ||
        artifact_die "HBF-disabled overhead run instantiated HBF"
    fi
    "$PYTHON" - "$wall" "$user" "$system" <<'PY'
import sys
if any(float(value) <= 0 for value in sys.argv[1:]):
    raise SystemExit(f"non-positive host timing: {sys.argv[1:]}")
PY
    printf '%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,0,1\n' \
      "$mode" "$repeat" "$ITERATIONS" "$ELEMENTS" "$cycles" "$wall" \
      "$user" "$system" "$rss" "$requests" >> "$OUT/overhead_summary.csv"
  done
done

capacity_config="$CONFIGS/sparse-capacity.config"
capacity_run="$OUT/sparse_capacity"
sed \
  -e 's/^-gpgpu_n_clusters .*/-gpgpu_n_clusters 8/' \
  -e 's/^-gpgpu_n_mem .*/-gpgpu_n_mem 8/' \
  -e 's/^-gpgpu_gmem_skip_L1D .*/-gpgpu_gmem_skip_L1D 1/' \
  -e 's/^-gpgpu_hbf_enabled .*/-gpgpu_hbf_enabled 1/' \
  -e "s/^-gpgpu_hbf_size .*/-gpgpu_hbf_size $CAPACITY_BYTES/" \
  -e "s/^-gpgpu_hbf_num_stacks .*/-gpgpu_hbf_num_stacks $STACKS/" \
  -e 's/^-gpgpu_hbf_stack_map .*/-gpgpu_hbf_stack_map 0/' \
  -e "s/^-gpgpu_hbf_num_channels .*/-gpgpu_hbf_num_channels $CHANNELS/" \
  -e "s/^-gpgpu_hbf_num_subarrays .*/-gpgpu_hbf_num_subarrays $SUBARRAYS/" \
  -e 's/^-gpgpu_hbf_max_active .*/-gpgpu_hbf_max_active 32/' \
  -e 's/^-gpgpu_hbf_tR .*/-gpgpu_hbf_tR 20/' \
  -e 's/^-gpgpu_hbf_tPROG .*/-gpgpu_hbf_tPROG 40/' \
  -e 's/^-gpgpu_hbf_tBERS .*/-gpgpu_hbf_tBERS 80/' \
  -e 's/^-gpgpu_hbf_cache_entries .*/-gpgpu_hbf_cache_entries 0/' \
  -e "s|^-gpgpu_hbf_trace_file .*|-gpgpu_hbf_trace_file \"$capacity_run/hbf.csv\"|" \
  -e 's/^-gpgpu_hbf_trace_level .*/-gpgpu_hbf_trace_level 1/' \
  -e 's/^-gpgpu_deadlock_detect .*/-gpgpu_deadlock_detect 0/' \
  "$BASE_CONFIG" > "$capacity_config"
grep -Fxq -- "-gpgpu_hbf_size $CAPACITY_BYTES" "$capacity_config"
grep -Fxq -- "-gpgpu_hbf_num_stacks $STACKS" "$capacity_config"

"$HERE/../common/run_sim.sh" "$capacity_run" "$capacity_config" \
  "$BIN/sparse_capacity_probe" "$HBF_BASE" "$CAPACITY_BYTES"
assert_run_success "$capacity_run" \
  "[SPARSE-CAPACITY] completed: 5 pages across $CAPACITY_BYTES bytes"
trace_requests="$(validate_trace_group "$VALIDATOR" "$capacity_run/hbf.csv" \
  "$STACKS" "$CHANNELS" "$SUBARRAYS" "$capacity_run/trace_validation")"
[[ "$trace_requests" == 20 ]] || artifact_die "capacity probe expected 20 sector requests"

"$PYTHON" - "$capacity_run" "$HBF_BASE" "$CAPACITY_BYTES" "$STACKS" <<'PY'
import csv
import sys
from pathlib import Path

run_dir = Path(sys.argv[1])
base, span, stacks = map(int, sys.argv[2:])
total_pages = span // 4096
global_pages = [0, total_pages // 4 + 1, total_pages // 2 + 2,
                3 * total_pages // 4 + 3, total_pages - 1]
expected = {stack: set() for stack in range(stacks)}
for page in global_pages:
    expected[page % stacks].add(page // stacks)
observed = {}
for stack in range(stacks):
    path = Path(f"{run_dir}/hbf.csv.stack{stack}.csv")
    with path.open(newline="") as stream:
        rows = [row for row in csv.DictReader(stream) if row["state"] == "INGRESS"]
    if any(row["op"] != "R" or int(row["bytes"]) != 32 for row in rows):
        raise SystemExit(f"stack {stack}: capacity probe has malformed ingress")
    observed[stack] = {int(row["page"]) for row in rows}
if observed != expected:
    raise SystemExit(f"capacity page mismatch: expected={expected}, observed={observed}")
PY

total_capacity="$(awk '/^HBF Total Capacity:/ {print $4}' "$capacity_run/run.log" | tail -1)"
per_stack_capacity="$(awk '/^HBF Per-Stack Capacity:/ {print $4}' "$capacity_run/run.log" | tail -1)"
requests="$(sum_stat "$capacity_run/run.log" 'HBF Total Requests:')"
allocations="$(sum_stat "$capacity_run/run.log" 'HBF FTL Allocations:')"
metadata_pages="$(sum_stat "$capacity_run/run.log" 'HBF Allocated Metadata:')"
mapping_errors="$(sum_stat "$capacity_run/run.log" 'HBF Mapping Errors:')"
capacity_ok="$(grep -Fc 'HBF FTL Capacity Error:  no' "$capacity_run/run.log")"
configured_pages="$(awk '/^HBF Capacity Usage:/ {sum += $7} END {print sum}' "$capacity_run/run.log")"
[[ "$total_capacity" == "$CAPACITY_BYTES" ]] || artifact_die "wrong total capacity"
[[ "$per_stack_capacity" == $((CAPACITY_BYTES / STACKS)) ]] || artifact_die "wrong per-stack capacity"
[[ "$configured_pages" == $((CAPACITY_BYTES / PAGE_SIZE)) ]] || artifact_die "wrong configured-page total"
[[ "$requests" == 20 && "$allocations" == 0 && "$metadata_pages" == 0 && "$mapping_errors" == 0 ]] ||
  artifact_die "capacity invariants failed"
[[ "$capacity_ok" == "$STACKS" ]] || artifact_die "one or more stacks reported capacity failure"

printf 'total_capacity_bytes,per_stack_capacity_bytes,configured_pages,stacks,channels_per_stack,touched_global_pages,trace_requests,ftl_allocations,allocated_metadata_pages,mapping_errors,capacity_error_stacks,run_rc,trace_valid\n' \
  > "$OUT/capacity_summary.csv"
printf '%s,%s,%s,%s,%s,5,%s,%s,%s,%s,0,0,1\n' \
  "$total_capacity" "$per_stack_capacity" "$configured_pages" "$STACKS" \
  "$CHANNELS" "$trace_requests" "$allocations" "$metadata_pages" \
  "$mapping_errors" >> "$OUT/capacity_summary.csv"

"$PYTHON" - "$OUT/overhead_summary.csv" "$OUT/capacity_summary.csv" \
  "$OUT/summary.json" <<'PY'
import csv
import json
import statistics
import sys
from pathlib import Path

overhead_path, capacity_path, output_path = map(Path, sys.argv[1:])
with overhead_path.open(newline="") as stream:
    overhead = list(csv.DictReader(stream))
with capacity_path.open(newline="") as stream:
    capacity = list(csv.DictReader(stream))
groups = {
    mode: [row for row in overhead if row["mode"] == mode]
    for mode in ("disabled", "enabled")
}
if any(len(rows) == 0 for rows in groups.values()) or len(capacity) != 1:
    raise SystemExit("cost/capacity evidence matrix is incomplete")
cycles = {int(row["cycles"]) for row in overhead}
if len(cycles) != 1:
    raise SystemExit(f"HBF instrumentation changed simulated cycles: {cycles}")
medians = {
    mode: {
        "wall_seconds": statistics.median(float(row["wall_seconds"]) for row in rows),
        "max_rss_kib": statistics.median(int(row["max_rss_kib"]) for row in rows),
    }
    for mode, rows in groups.items()
}
summary = {
    "schema": "hbf-cost-capacity-v1",
    "overhead_repeats_per_mode": len(groups["disabled"]),
    "simulated_cycles": cycles.pop(),
    "medians": medians,
    "wall_time_ratio_enabled_to_disabled": (
        medians["enabled"]["wall_seconds"] / medians["disabled"]["wall_seconds"]
    ),
    "rss_ratio_enabled_to_disabled": (
        medians["enabled"]["max_rss_kib"] / medians["disabled"]["max_rss_kib"]
    ),
    "capacity": capacity[0],
    "all_runs_valid": True,
    "source_csvs": [str(overhead_path.resolve()), str(capacity_path.resolve())],
}
output_path.write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n")
PY

PROVENANCE_COMMAND="RUN_TAG=$TAG make reproduce-paper PAPER_RQ=cost" \
  "$ROOT/scripts/record_provenance.sh" "$OUT" \
  "$ROOT/Makefile" "$ROOT/artifact/integration.env" \
  "$ROOT/artifact/hbf_sources.txt" "$ROOT/$GPGPUSIM_PATCH" \
  "$ROOT/hbf/gpgpusim_hbf.config" "$HERE/run.sh" \
  "$HERE/overhead_probe.cu" "$HERE/sparse_capacity_probe.cu" "$VALIDATOR" \
  "$HERE/../common/run_with_metrics.py"

cat > "$OUT/README.txt" <<EOF
HBF-Sim host-cost and sparse-capacity validation bundle
UTC tag: $TAG
The overhead matrix runs the same DRAM-only CUDA kernel with HBF disabled and
enabled, alternating order across $REPEATS repetitions. Simulated cycles must
remain identical and the HBF-enabled case must route zero requests to HBF.
The capacity probe reads five pages from the first through final page of the
512 GiB logical range across four stacks. Every stack must receive traffic,
all 20 sector requests must complete, and read-only access must allocate no
FTL blocks or page metadata.
EOF
echo "cost/capacity bundle: $OUT"
