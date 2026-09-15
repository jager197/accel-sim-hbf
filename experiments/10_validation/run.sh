#!/usr/bin/env bash
# Fast end-to-end validation for the logical HBF stack.
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
BIN="$OUT/workloads_bin"
CONFIGS="$OUT/configs"
VALIDATOR="$ROOT/util/traces/validate_hbf_trace.py"
BASE_CONFIG="$GPGPUSIM_ROOT/configs/tested-cfgs/SM7_QV100/gpgpusim_hbf.config"

validate_run_tag "$TAG"
prepare_fresh_output "$OUT"
mkdir "$BIN" "$CONFIGS"
require_executable "$CUDA_INSTALL_PATH/bin/nvcc"
require_file "$BASE_CONFIG"
require_file "$VALIDATOR"

"$CUDA_INSTALL_PATH/bin/nvcc" $SM70 --cudart shared -o "$BIN/shared_staging" \
  "$HERE/../workloads/shared_staging.cu"
"$CUDA_INSTALL_PATH/bin/nvcc" $SM70 --cudart shared -o "$BIN/channel_probe" \
  "$HERE/../workloads/channel_probe.cu"
"$CUDA_INSTALL_PATH/bin/nvcc" $SM70 --cudart shared -o "$BIN/vector_add" \
  "$HERE/../workloads/vector_add.cu"
require_executable "$BIN/shared_staging"
require_executable "$BIN/channel_probe"
require_executable "$BIN/vector_add"

make_config() {
  local name="$1" channels="$2" read_mode="$3" placement="$4"
  local run_dir="$OUT/$name" config="$CONFIGS/$name.config"
  sed \
    -e 's/^-gpgpu_n_clusters .*/-gpgpu_n_clusters 8/' \
    -e 's/^-gpgpu_n_mem .*/-gpgpu_n_mem 8/' \
    -e 's/^-gpgpu_gmem_skip_L1D .*/-gpgpu_gmem_skip_L1D 1/' \
    -e 's/^-gpgpu_dram_partition_queues .*/-gpgpu_dram_partition_queues 4096:4096:4096:4096/' \
    -e 's/^-gpgpu_frfcfs_dram_sched_queue_size .*/-gpgpu_frfcfs_dram_sched_queue_size 4096/' \
    -e 's/^-gpgpu_dram_return_queue_size .*/-gpgpu_dram_return_queue_size 4096/' \
    -e 's/^-gpgpu_hbf_l2_policy .*/-gpgpu_hbf_l2_policy 0/' \
    -e 's/^-gpgpu_hbf_num_stacks .*/-gpgpu_hbf_num_stacks 1/' \
    -e 's/^-gpgpu_hbf_stack_map .*/-gpgpu_hbf_stack_map 0/' \
    -e 's/^-gpgpu_hbf_num_subarrays .*/-gpgpu_hbf_num_subarrays 32/' \
    -e 's/^-gpgpu_hbf_max_active .*/-gpgpu_hbf_max_active 8/' \
    -e 's/^-gpgpu_hbf_tR .*/-gpgpu_hbf_tR 20/' \
    -e 's/^-gpgpu_hbf_tPROG .*/-gpgpu_hbf_tPROG 40/' \
    -e 's/^-gpgpu_hbf_tBERS .*/-gpgpu_hbf_tBERS 80/' \
    -e "s/^-gpgpu_hbf_num_channels .*/-gpgpu_hbf_num_channels $channels/" \
    -e "s/^-gpgpu_hbf_placement_mode .*/-gpgpu_hbf_placement_mode $placement/" \
    -e "s/^-gpgpu_hbf_read_mode .*/-gpgpu_hbf_read_mode $read_mode/" \
    -e "s|^-gpgpu_hbf_trace_file .*|-gpgpu_hbf_trace_file \"$run_dir/hbf.csv\"|" \
    -e 's/^-gpgpu_hbf_trace_level .*/-gpgpu_hbf_trace_level 1/' \
    -e 's/^-gpgpu_deadlock_detect .*/-gpgpu_deadlock_detect 0/' \
    "$BASE_CONFIG" > "$config"
  grep -Fxq -- "-gpgpu_hbf_num_channels $channels" "$config"
  grep -Fxq -- '-gpgpu_hbf_num_stacks 1' "$config"
  printf '%s\n' "$config"
}

validate_run_trace() {
  local run_dir="$1" channels="$2" expected="${3:-}"
  local args=("$run_dir/hbf.csv" --channels "$channels" --subarrays 32 \
    --require-nonempty --json "$run_dir/trace_validation.json")
  [[ -z "$expected" ]] || args+=(--expected-requests "$expected")
  "$PYTHON" "$VALIDATOR" "${args[@]}" > "$run_dir/trace_validation.txt"
  validate_trace_report "$run_dir/trace_validation.json"
}

dram_config="$CONFIGS/dram_baseline.config"
sed \
  -e 's/^-gpgpu_n_clusters .*/-gpgpu_n_clusters 8/' \
  -e 's/^-gpgpu_n_mem .*/-gpgpu_n_mem 8/' \
  -e 's/^-gpgpu_hbf_enabled .*/-gpgpu_hbf_enabled 0/' \
  -e 's/^-gpgpu_deadlock_detect .*/-gpgpu_deadlock_detect 0/' \
  "$BASE_CONFIG" > "$dram_config"
grep -Fxq -- '-gpgpu_hbf_enabled 0' "$dram_config"
grep -Fxq -- '-gpgpu_deadlock_detect 0' "$dram_config"
dram_run="$OUT/dram_baseline"
"$HERE/../common/run_sim.sh" "$dram_run" "$dram_config" "$BIN/vector_add"
assert_run_success "$dram_run" 'Test PASSED: 0 errors out of 1024'
if grep -Fq '========= HBF System Statistics =========' "$dram_run/run.log"; then
  artifact_die "HBF-disabled DRAM baseline unexpectedly instantiated HBF"
fi
dram_cycles="$(last_stat "$dram_run/run.log" 'gpu_sim_cycle =')"
require_positive_value cycles "$dram_cycles"
printf 'mode,cycles,hbf_enabled,numerical_valid,run_rc\n' > "$OUT/dram_summary.csv"
printf 'dram,%s,0,1,0\n' "$dram_cycles" >> "$OUT/dram_summary.csv"

printf 'channels,cycles,hbf_requests,page_reads,mshr_hits,trace_requests,trace_valid\n' \
  > "$OUT/channel_summary.csv"
for channels in 1 2 4 8 16; do
  name="channel_$channels"
  run_dir="$OUT/$name"
  config="$(make_config "$name" "$channels" 0 0)"
  "$HERE/../common/run_sim.sh" "$run_dir" "$config" "$BIN/channel_probe" \
    "$HBF_BASE" 64 4096
  assert_run_success "$run_dir" '[CHANNEL-PROBE] completed: 64'
  validate_run_trace "$run_dir" "$channels" 64
  cycles="$(last_stat "$run_dir/run.log" 'gpu_sim_cycle =')"
  requests="$(sum_stat "$run_dir/run.log" 'HBF Total Requests:')"
  pages="$(sum_stat "$run_dir/run.log" 'HBF Page Reads:')"
  hits="$(sum_stat "$run_dir/run.log" 'HBF MSHR Hits:')"
  trace_requests="$($PYTHON -c 'import json,sys; print(json.load(open(sys.argv[1]))["request_ids"])' "$run_dir/trace_validation.json")"
  require_positive_value cycles "$cycles"
  require_positive_value hbf_requests "$requests"
  require_positive_value page_reads "$pages"
  require_positive_value trace_requests "$trace_requests"
  printf '%s,%s,%s,%s,%s,%s,1\n' \
    "$channels" "$cycles" "$requests" "$pages" "$hits" "$trace_requests" \
    >> "$OUT/channel_summary.csv"
done

"$PYTHON" "$HERE/plot_validation.py" "$OUT/channel_summary.csv" --out-dir "$OUT"

printf 'mode,cycles,hbf_requests,page_reads,trace_requests,trace_valid\n' \
  > "$OUT/read_summary.csv"
for mode in demand aggregation; do
  read_mode=0
  [[ "$mode" == aggregation ]] && read_mode=1
  name="read_$mode"
  run_dir="$OUT/$name"
  config="$(make_config "$name" 4 "$read_mode" 0)"
  sed -i \
    -e 's/^-gpgpu_hbf_read_agg_window .*/-gpgpu_hbf_read_agg_window 10/' \
    -e 's/^-gpgpu_hbf_read_agg_threshold .*/-gpgpu_hbf_read_agg_threshold 4096/' \
    "$config"
  "$HERE/../common/run_sim.sh" "$run_dir" "$config" "$BIN/shared_staging" \
    "$HBF_BASE" 256
  assert_run_success "$run_dir" '[SHARED-STAGING] completed: 256'
  validate_run_trace "$run_dir" 4
  cycles="$(last_stat "$run_dir/run.log" 'gpu_sim_cycle =')"
  requests="$(sum_stat "$run_dir/run.log" 'HBF Total Requests:')"
  pages="$(sum_stat "$run_dir/run.log" 'HBF Page Reads:')"
  trace_requests="$($PYTHON -c 'import json,sys; print(json.load(open(sys.argv[1]))["request_ids"])' "$run_dir/trace_validation.json")"
  require_positive_value cycles "$cycles"
  require_positive_value hbf_requests "$requests"
  require_positive_value page_reads "$pages"
  require_positive_value trace_requests "$trace_requests"
  printf '%s,%s,%s,%s,%s,1\n' \
    "$mode" "$cycles" "$requests" "$pages" "$trace_requests" \
    >> "$OUT/read_summary.csv"
done

PROVENANCE_COMMAND="RUN_TAG=$TAG make reproduce-smoke" \
  "$ROOT/scripts/record_provenance.sh" "$OUT" \
  "$ROOT/Makefile" "$ROOT/artifact/integration.env" \
  "$ROOT/artifact/hbf_sources.txt" "$ROOT/$GPGPUSIM_PATCH" \
  "$ROOT/hbf/gpgpusim_hbf.config" "$HERE/run.sh" "$HERE/plot_validation.py" \
  "$HERE/../workloads/channel_probe.cu" "$HERE/../workloads/shared_staging.cu" \
  "$HERE/../workloads/vector_add.cu" \
  "$VALIDATOR"

cat > "$OUT/README.txt" <<EOF
HBF-Sim fast validation bundle
UTC tag: $TAG
The HBF-disabled DRAM numerical baseline and all seven HBF runs completed
successfully. HBF runs reported positive traffic and passed exact
hbf-trace-v1 lifecycle validation. This directory is immutable; reruns require
a new RUN_TAG.
EOF
echo "validation bundle: $OUT"
