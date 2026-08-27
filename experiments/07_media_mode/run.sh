#!/bin/bash
# ============================================================================
# 07_media_mode — E3: HBF 介质语义 vs SSD 语义（论文 §5.2 核心对比图）
#
# 问题: OCP §11.4 明确 HBF 没有设备侧 GC、不允许搬移有效数据；SSD 派生的
# FTL+GC 模型会引入规范中不存在的后台流量。E3 在完全相同配置下只切换
# -gpgpu_hbf_media_mode（0=hbf / 1=ssd），量化 SSD 语义的误判:
#   GC 事件数、有效页搬移、写放大（页编程/逻辑写）、擦除次数、
#   前台读尾延迟、端到端周期差。
#
# 负载: hbf_overwrite（覆盖写——使旧物理页失效，SSD 模式下 GC 必须搬移
# 有效页；HBF 模式下全失效块直接回收，无搬移）。kv_write（追加写）作为
# 无失效对照。
#
# 用法: bash experiments/07_media_mode/run.sh [ENTRY_LIST] [ITERS]
# 产物: results/results.csv, results/results.md
# ============================================================================
set -u
source "$(dirname "$0")/../common/start_logging.sh" "$0"
HERE="$(cd "$(dirname "$0")" && pwd)"
source "$HERE/../common/env.sh"

ENTRY_LIST="${1:-1024,2048}"
ITERS="${2:-1}"
MAX_PAR="${MAX_PAR:-4}"
RUN_TIMEOUT="${RUN_TIMEOUT:-5400}"

RUNS="$HERE/runs"
BIN="$HERE/../workloads_bin"
mkdir -p "$RUNS" "$BIN" "$HERE/results"
source "$HERE/../configs/gen_configs.sh" >/dev/null

nvcc $SM70 --cudart shared -o "$BIN/hbf_overwrite" "$HERE/../workloads/hbf_overwrite.cu"
nvcc $SM70 --cudart shared -o "$BIN/kv_write" "$HERE/../workloads/kv_write.cu"
echo "workloads 已编译"

CMDS="$RUNS/cmds.txt"; : > "$CMDS"
for W in $(echo "$ENTRY_LIST" | tr ',' ' '); do
    # 覆盖写（触发 GC 差异）；smallblk: pages_per_block=4 使块快速填满、
    # SSD 模式的 GC 必须搬移有效页（HBF 模式全失效块直接回收）
    echo "$RUNS/hbf_ow_${W}e  $CFG_DIR/limited_hbf_smallblk.config   $BIN/hbf_overwrite $W $HBF_BASE" >> "$CMDS"
    echo "$RUNS/ssd_ow_${W}e  $CFG_DIR/hbf_ssd_mode_smallblk.config $BIN/hbf_overwrite $W $HBF_BASE" >> "$CMDS"
done
echo "共 $(wc -l < "$CMDS") 个仿真（并发 $MAX_PAR）"
xargs -a "$CMDS" -P "$MAX_PAR" -L 1 bash "$HERE/../common/run_sim.sh"

CSV="$HERE/results/results.csv"
{
    echo -n "mode,entries,iters,"
    python3 "$HERE/../common/parse_stats.py" --csv-header
} > "$CSV"
for W in $(echo "$ENTRY_LIST" | tr ',' ' '); do
    for MODE in hbf ssd; do
        LOG="$RUNS/${MODE}_ow_${W}e/run.log"
        [ -s "$LOG" ] || { echo "  !! 缺少 $LOG"; continue; }
        python3 "$HERE/../common/parse_stats.py" --csv "$LOG" "$MODE" "$W" "$ITERS" >> "$CSV"
    done
done
echo "结果: $CSV"
echo "07_media_mode 完成"
