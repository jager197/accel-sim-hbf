#!/bin/bash
# ============================================================================
# experiments/common/start_logging.sh — 终端输出自动落盘
#
# 用法：在各实验 run.sh 的 set -u 之后立即 source：
#     set -u
#     source "$(dirname "$0")/../common/start_logging.sh" "$0"
#
# 效果：stdout+stderr 全部 tee 到 experiments/logs/<脚本名>.log（每次运行
#       覆盖），终端照常显示。嵌套调用时由最外层脚本落盘，内层不再重复。
# 仿真本身的输出由 common/run_sim.sh 写入各 runs/<name>/run.log。
# ============================================================================

if [ -n "${_HBF_LOG_ACTIVE:-}" ]; then
    return 0 2>/dev/null || exit 0
fi

_SCRIPT_NAME="$(basename "${1:-$0}")"
# 向上定位 experiments/ 目录（无论脚本在 experiments/ 还是其子目录）
_D="$(cd "$(dirname "${1:-$0}")" && pwd)"
_D_PARENT="$(basename "$_D")"
while [ "$(basename "$_D")" != "experiments" ] && [ "$_D" != "/" ]; do
    _D="$(dirname "$_D")"
done
_LOGDIR="$_D/logs"
mkdir -p "$_LOGDIR"
# 日志名: 顶层脚本用自身名（run_all.sh），步骤脚本用步骤目录名（02_kv_cache 等）
if [ "$_SCRIPT_NAME" = "run.sh" ]; then
    _SCRIPT_NAME="$_D_PARENT.sh"
fi
_LOGFILE="$_LOGDIR/${_SCRIPT_NAME}.log"

export _HBF_LOG_ACTIVE=1
{
    echo "======================================================================"
    echo " $(basename "$_SCRIPT_NAME") — $(date '+%Y-%m-%d %H:%M:%S')"
    echo " 命令: $(basename "$_SCRIPT_NAME") ${*:2}"
    echo "======================================================================"
} > "$_LOGFILE"
exec > >(tee -a "$_LOGFILE") 2>&1
