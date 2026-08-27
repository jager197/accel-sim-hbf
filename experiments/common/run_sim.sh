#!/bin/bash
# ============================================================================
# experiments/common/run_sim.sh — 单次 GPGPU-Sim 仿真封装
#
# 用法: run_sim.sh <run_dir> <config> <binary> [args...]
#   run_dir   运行目录（自动创建；若存在且 run.log 有效则跳过 → 幂等重跑）
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
#   FORCE_RERUN=1 强制重跑（默认跳过已有成功 run.log 的运行）
# ============================================================================

RUN_TIMEOUT="${RUN_TIMEOUT:-1800}"
FORCE_RERUN="${FORCE_RERUN:-0}"

run_dir="$1"; config="$2"; binary="$3"; shift 3

if [ -z "$run_dir" ] || [ -z "$config" ] || [ -z "$binary" ]; then
    echo "usage: run_sim.sh <run_dir> <config> <binary> [args...]" >&2
    exit 2
fi

# 幂等：已成功跑过且不强制重跑 → 跳过
if [ "$FORCE_RERUN" != "1" ] && [ -f "$run_dir/run.rc" ] \
   && [ "$(cat "$run_dir/run.rc")" = "0" ] \
   && [ -s "$run_dir/run.log" ]; then
    echo "skip  $run_dir (cached, FORCE_RERUN=1 to redo)"
    exit 0
fi

mkdir -p "$run_dir"
ln -sf "$(cd "$(dirname "$config")" && pwd)/$(basename "$config")" \
    "$run_dir/gpgpusim.config"
ln -sf "$ICXT_CFG" "$run_dir/config_volta_islip.icnt" 2>/dev/null || true

echo "run   $run_dir  <- $(basename "$config")"
( cd "$run_dir" && timeout "$RUN_TIMEOUT" "$binary" "$@" > run.log 2>&1 )
rc=$?
echo "$rc" > "$run_dir/run.rc"
if [ "$rc" != "0" ]; then
    echo "  !! exit=$rc (timeout=$RUN_TIMEOUT s)  see $run_dir/run.log"
fi
exit 0
