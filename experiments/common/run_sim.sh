#!/usr/bin/env bash
# ============================================================================
# experiments/common/run_sim.sh — 单次 GPGPU-Sim 仿真封装
#
# 用法: run_sim.sh <run_dir> <config> <binary> [args...]
#   run_dir   运行目录（必须不存在；脚本拒绝覆盖或复用结果）
#   config    gpgpusim.config 的路径（被符号链接进 run_dir）
#   binary    要运行的可执行文件（绝对或相对路径）
#   args...   传给 binary 的参数
#
# 行为:
#   1. mkdir -p run_dir，链接 gpgpusim.config + config_volta_islip.icnt
#   2. 在 run_dir 内执行 binary，输出写入 run.log
#   3. 退出码写入 run.rc；超时由 RUN_TIMEOUT 控制（默认 1800 秒）
#
# 环境变量:
#   RUN_TIMEOUT   单次仿真超时秒数（默认 1800）
#   FORCE_RERUN   已废弃；只接受 0，重跑必须使用新目录/tag
# ============================================================================

set -uo pipefail

RUN_TIMEOUT="${RUN_TIMEOUT:-1800}"
FORCE_RERUN="${FORCE_RERUN:-0}"
PYTHON="${PYTHON:-python3}"
METRICS_RUNNER="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/run_with_metrics.py"

if (( $# < 3 )); then
    echo "usage: run_sim.sh <run_dir> <config> <binary> [args...]" >&2
    exit 2
fi
run_dir="$1"; config="$2"; binary="$3"; shift 3

if [[ ! "$RUN_TIMEOUT" =~ ^[1-9][0-9]*$ ]]; then
    echo "run_sim: RUN_TIMEOUT must be a positive integer" >&2
    exit 2
fi
if [[ "$FORCE_RERUN" != "0" ]]; then
    echo "run_sim: FORCE_RERUN is unsupported; use a fresh run directory" >&2
    exit 2
fi
[[ -f "$config" ]] || { echo "run_sim: config is missing: $config" >&2; exit 1; }
[[ -x "$binary" ]] || { echo "run_sim: binary is not executable: $binary" >&2; exit 1; }
[[ -f "$METRICS_RUNNER" ]] || { echo "run_sim: metrics runner is missing: $METRICS_RUNNER" >&2; exit 1; }
[[ -n "${ICXT_CFG:-}" && -f "$ICXT_CFG" ]] || {
    echo "run_sim: ICXT_CFG is unset or missing" >&2
    exit 1
}
for required in timeout sha256sum "$PYTHON"; do
    command -v "$required" >/dev/null 2>&1 || {
        echo "run_sim: required command is missing: $required" >&2
        exit 1
    }
done
config_abs="$(cd "$(dirname "$config")" && pwd)/$(basename "$config")"
binary_abs="$(cd "$(dirname "$binary")" && pwd)/$(basename "$binary")"
icnt_abs="$(cd "$(dirname "$ICXT_CFG")" && pwd)/$(basename "$ICXT_CFG")"
input_fingerprint="$({
    sha256sum "$config_abs" "$binary_abs" "$icnt_abs"
    printf 'arg=%q\n' "$@"
} | sha256sum | awk '{print $1}')"

if [[ -e "$run_dir" ]]; then
    echo "run_sim: refusing existing run directory: $run_dir" >&2
    echo "run_sim: choose a fresh tag" >&2
    exit 2
fi

mkdir -p "$run_dir"
ln -sf "$config_abs" "$run_dir/gpgpusim.config"
ln -sf "$icnt_abs" "$run_dir/config_volta_islip.icnt"
printf '%s\n' "$input_fingerprint" > "$run_dir/run.input.sha256"

# Preserve the inputs needed to reproduce a run.  This metadata is deliberately
# plain text so it remains readable in an archived experiment bundle.
{
    printf 'timestamp_utc=%s\n' "$(date -u '+%Y-%m-%dT%H:%M:%SZ')"
    printf 'run_dir=%s\n' "$(cd "$run_dir" && pwd)"
    printf 'config=%s\n' "$config_abs"
    printf 'config_sha256=%s\n' "$(sha256sum "$config_abs" | awk '{print $1}')"
    printf 'binary=%s\n' "$binary_abs"
    if [ -f "$binary_abs" ]; then
        printf 'binary_sha256=%s\n' "$(sha256sum "$binary_abs" | awk '{print $1}')"
    else
        printf 'binary_sha256=missing\n'
    fi
    printf 'interconnect=%s\n' "$icnt_abs"
    printf 'interconnect_sha256=%s\n' "$(sha256sum "$icnt_abs" | awk '{print $1}')"
    printf 'input_fingerprint=%s\n' "$input_fingerprint"
    if [ -n "${REPO_ROOT:-}" ] && [ -d "$REPO_ROOT/.git" ]; then
        printf 'repo_git_head=%s\n' "$(git -C "$REPO_ROOT" rev-parse HEAD 2>/dev/null || echo unknown)"
        printf 'repo_git_dirty=%s\n' "$(git -C "$REPO_ROOT" status --porcelain 2>/dev/null | wc -l)"
    fi
    if [ -n "${GPGPUSIM_ROOT:-}" ] && [ -d "$GPGPUSIM_ROOT/.git" ]; then
        printf 'gpgpusim_git_head=%s\n' "$(git -C "$GPGPUSIM_ROOT" rev-parse HEAD 2>/dev/null || echo unknown)"
        printf 'gpgpusim_git_dirty=%s\n' "$(git -C "$GPGPUSIM_ROOT" status --porcelain 2>/dev/null | wc -l)"
    fi
    printf 'cuda_install_path=%s\n' "${CUDA_INSTALL_PATH:-unknown}"
    if command -v nvcc >/dev/null 2>&1; then
        printf 'cuda_compiler=%s\n' "$(nvcc --version 2>/dev/null | awk -F'release ' '/release/{print $2; exit}' | tr -d ',')"
    else
        printf 'cuda_compiler=missing\n'
    fi
    printf 'command='
    printf '%q ' "$binary_abs" "$@"
    printf '\n'
} > "$run_dir/run.meta"

echo "run   $run_dir  <- $(basename "$config")"
( cd "$run_dir" && \
  "$PYTHON" "$METRICS_RUNNER" run.time timeout "$RUN_TIMEOUT" \
    "$binary_abs" "$@" > run.log 2>&1 )
rc=$?
echo "$rc" > "$run_dir/run.rc"
if [ "$rc" != "0" ]; then
    echo "  !! exit=$rc (timeout=$RUN_TIMEOUT s)  see $run_dir/run.log"
fi
exit "$rc"
