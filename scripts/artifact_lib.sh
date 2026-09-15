#!/usr/bin/env bash

# Shared fail-closed helpers for artifact experiment runners.

artifact_die() {
  printf 'artifact: %s\n' "$*" >&2
  return 1
}

require_command() {
  command -v "$1" >/dev/null 2>&1 || artifact_die "required command is missing: $1"
}

require_file() {
  [[ -f "$1" ]] || artifact_die "required file is missing: $1"
}

require_executable() {
  [[ -x "$1" ]] || artifact_die "required executable is missing: $1"
}

require_positive_integer() {
  local name="$1" value="$2"
  [[ "$value" =~ ^[1-9][0-9]*$ ]] || artifact_die "$name must be a positive integer: $value"
}

validate_run_tag() {
  local tag="$1"
  [[ "$tag" =~ ^[A-Za-z0-9][A-Za-z0-9._-]{0,79}$ ]] ||
    artifact_die "RUN_TAG must start with an alphanumeric and contain at most 80 safe characters: $tag"
}

prepare_fresh_output() {
  local output="$1"
  mkdir -p "$(dirname "$output")"
  mkdir "$output" 2>/dev/null ||
    artifact_die "refusing to reuse existing output directory: $output"
}

assert_run_success() {
  local run_dir="$1" marker="${2:-}"
  require_file "$run_dir/run.rc"
  [[ "$(tr -d '[:space:]' < "$run_dir/run.rc")" == "0" ]] ||
    artifact_die "simulation failed; see $run_dir/run.log"
  [[ -s "$run_dir/run.log" ]] || artifact_die "simulation log is empty: $run_dir/run.log"
  if [[ -n "$marker" ]]; then
    grep -Fq "$marker" "$run_dir/run.log" ||
      artifact_die "completion marker '$marker' is absent from $run_dir/run.log"
  fi
}

last_stat() {
  hbf_stat "$1" "$2" last
}

sum_stat() {
  hbf_stat "$1" "$2" sum
}

hbf_stat() {
  local log="$1" label="$2" mode="${3:-last}"
  "${PYTHON:-python3}" - "$log" "$label" "$mode" <<'PY'
import re
import sys
from pathlib import Path

path = Path(sys.argv[1])
label = sys.argv[2]
mode = sys.argv[3]
text = path.read_text(encoding="utf-8", errors="replace")
if label in {"gpu_sim_cycle =", "gpu_tot_sim_cycle ="}:
    values = [int(value) for value in re.findall(r"^" + re.escape(label) + r"\s*([0-9]+)", text, re.M)]
    if not values:
        raise SystemExit(f"stat '{label}' is absent from {path}")
    print(values[-1])
    raise SystemExit

system = text.rfind("========= HBF System Statistics =========")
if system >= 0:
    snapshot = text[system:]
    blocks = snapshot.split("========= HBF Cube Controller Statistics =========")[1:]
    if not blocks:
        blocks = [snapshot]
else:
    cube = text.rfind("========= HBF Cube Controller Statistics =========")
    blocks = [text[cube:] if cube >= 0 else text]

pattern = re.compile(r"^" + re.escape(label) + r"\s*(?:=|:)??\s*([0-9]+)", re.M)
values = [int(match.group(1)) for block in blocks for match in pattern.finditer(block)]
if not values:
    raise SystemExit(f"stat '{label}' is absent from {path}")
print(sum(values) if mode == "sum" else values[-1])
PY
}

require_positive_value() {
  local name="$1" value="$2"
  [[ "$value" =~ ^[0-9]+$ ]] && (( value > 0 )) ||
    artifact_die "$name must be positive, observed: ${value:-missing}"
}

validate_trace_report() {
  local report="$1"
  "${PYTHON:-python3}" - "$report" <<'PY'
import json
import sys

path = sys.argv[1]
with open(path, encoding="utf-8") as stream:
    report = json.load(stream)
if not report.get("valid"):
    raise SystemExit(f"invalid lifecycle trace {path}: {report.get('errors', [])}")
if not report.get("schema_exact") or report.get("schema_version") != "hbf-trace-v1":
    raise SystemExit(f"trace schema is not frozen v1: {path}")
if report.get("rows", 0) <= 0 or report.get("request_ids", 0) <= 0:
    raise SystemExit(f"trace contains no HBF traffic: {path}")
PY
}

validate_trace_group() {
  local validator="$1" trace_base="$2" stacks="$3" channels="$4"
  local subarrays="$5" report_prefix="$6" stable="${7:-0}"
  local stack trace report text count total=0
  require_positive_integer stacks "$stacks"
  for ((stack = 0; stack < stacks; ++stack)); do
    if (( stacks == 1 )); then
      trace="$trace_base"
      report="${report_prefix}.json"
      text="${report_prefix}.txt"
    else
      trace="${trace_base}.stack${stack}.csv"
      report="${report_prefix}.stack${stack}.json"
      text="${report_prefix}.stack${stack}.txt"
    fi
    require_file "$trace"
    args=("$trace" --channels "$channels" --subarrays "$subarrays" \
      --require-nonempty --json "$report")
    [[ "$stable" == "1" ]] && args+=(--require-stable-subarray)
    "${PYTHON:-python3}" "$validator" "${args[@]}" > "$text"
    validate_trace_report "$report"
    count="$("${PYTHON:-python3}" -c \
      'import json,sys; print(json.load(open(sys.argv[1]))["request_ids"])' "$report")"
    require_positive_value trace_requests "$count"
    total=$((total + count))
  done
  printf '%s\n' "$total"
}
