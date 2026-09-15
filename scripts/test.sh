#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PYTHON="${PYTHON:-python3}"
case "${1:-}" in ''|--tools-only) ;; *) exit 2 ;; esac
"$PYTHON" "$ROOT/scripts/check_release.py"
"$PYTHON" -m unittest discover -s "$ROOT/util/traces/tests" -p test_trace_tools.py -q
"$PYTHON" "$ROOT/experiments/reproduce.py" --plan --suite all >/dev/null
tmp_run="$(mktemp -d)"
trap 'rm -rf "$tmp_run"' EXIT
# GPU cumulative counters precede the HBF section, and are not per-stack sums.
cat > "$tmp_run/cumulative.log" <<'LOG'
gpu_sim_cycle = 4
gpu_tot_sim_cycle = 4
========= HBF System Statistics =========
========= HBF Cube Controller Statistics =========
HBF Total Requests: 32
gpu_sim_cycle = 5
gpu_tot_sim_cycle = 9
========= HBF System Statistics =========
========= HBF Cube Controller Statistics =========
HBF Total Requests: 64
LOG
source "$ROOT/scripts/artifact_lib.sh"
[[ "$(last_stat "$tmp_run/cumulative.log" 'gpu_tot_sim_cycle =')" == 9 ]]
[[ "$(last_stat "$tmp_run/cumulative.log" 'gpu_sim_cycle =')" == 5 ]]
[[ "$(sum_stat "$tmp_run/cumulative.log" 'HBF Total Requests:')" == 64 ]]
"$PYTHON" "$ROOT/artifact/figures/gen_media_saturation.py" --output-dir "$tmp_run/figures"
"$PYTHON" "$ROOT/artifact/figures/gen_case_studies.py" --output-dir "$tmp_run/figures"
"$PYTHON" "$ROOT/experiments/25_t02_t03_comparison/figures/gen_fig_comparison.py" --data-dir "$ROOT/artifact/reference" --output-dir "$tmp_run/figures"
printf 'config\n' > "$tmp_run/config"
printf 'icnt\n' > "$tmp_run/icnt"
printf '#!/usr/bin/env bash\nexit 7\n' > "$tmp_run/fail-bin"
chmod +x "$tmp_run/fail-bin"
set +e
ICXT_CFG="$tmp_run/icnt" "$ROOT/experiments/common/run_sim.sh" \
  "$tmp_run/result" "$tmp_run/config" "$tmp_run/fail-bin"
run_rc=$?
set -e
[[ "$run_rc" == 7 && "$(cat "$tmp_run/result/run.rc")" == 7 ]] || {
  echo "test: run_sim did not propagate exit status 7" >&2
  exit 1
}
grep -Fxq 'timer=python-resource' "$tmp_run/result/run.time"
grep -Fxq 'exit_code=7' "$tmp_run/result/run.time"
grep -Eq '^max_rss_kib=[1-9][0-9]*$' "$tmp_run/result/run.time"
set +e
ICXT_CFG="$tmp_run/icnt" "$ROOT/experiments/common/run_sim.sh" \
  "$tmp_run/result" "$tmp_run/config" "$tmp_run/fail-bin" >/dev/null 2>&1
reuse_rc=$?
set -e
[[ "$reuse_rc" == 2 ]] || {
  echo "test: run_sim reused an existing run directory" >&2
  exit 1
}

tmp="$tmp_run/provenance"
trap 'rm -rf "$tmp_run"' EXIT
PROVENANCE_COMMAND='make test' "$ROOT/scripts/record_provenance.sh" "$tmp" "$ROOT/Makefile" >/dev/null
grep -q '^repo_head=' "$tmp/provenance.env"
grep -q 'Makefile' "$tmp/inputs.sha256"

if [[ "${1:-}" != --tools-only ]]; then
  "$ROOT/hbf/tests/run_tests.sh"
fi
echo "test: release checks passed"
