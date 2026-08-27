#!/bin/bash
# ============================================================================
# 06_channel_scale — E1: Host Channel 数 × 有效带宽（论文 §6.1 主图）
#
# 问题: OCP HBF v0.7.0 的单 cube 最多 16 个 Host Channel（§4.5）。通道数
# 决定接口带宽上限（每通道 192 GB/s 有效）与 NAND 资源切片粒度。E1 扫描
# 每分区通道数 {1,2,4,8,16}（8 分区 → cube 等效 8..128 通道），回答:
#   RQ1 — 有效带宽何时受限于接口、何时受限于 NAND 并行、何时受限于
#   GPU 前端喂流（三条上限线的分离，见 docs/validation.md §4）。
#
# 配置: hbf_ch1..hbf_ch16（gen_configs.sh 生成，均含前端信用放大）
# 负载: weight_load（顺序流，1024 warps 8 路展开）
# 指标: 有效带宽（bytes / 总周期 × core 频率）、页读次数、每通道利用率
#
# 用法: bash experiments/06_channel_scale/run.sh [SIZE_MB]
# 产物: results/results.csv, results/results.md
# ============================================================================
set -u
source "$(dirname "$0")/../common/start_logging.sh" "$0"
HERE="$(cd "$(dirname "$0")" && pwd)"
source "$HERE/../common/env.sh"

SIZE_MB="${1:-4}"
MAX_PAR="${MAX_PAR:-5}"
RUN_TIMEOUT="${RUN_TIMEOUT:-3600}"

RUNS="$HERE/runs"
BIN="$HERE/../workloads_bin"
mkdir -p "$RUNS" "$BIN" "$HERE/results"
source "$HERE/../configs/gen_configs.sh" >/dev/null

nvcc $SM70 --cudart shared -o "$BIN/weight_load" "$HERE/../workloads/weight_load.cu"

CMDS="$RUNS/cmds.txt"; : > "$CMDS"
for NCH in 1 2 4 8 16; do
    echo "$RUNS/ch${NCH}_${SIZE_MB}mb  $CFG_DIR/hbf_ch${NCH}.config  $BIN/weight_load $SIZE_MB 1 $HBF_BASE" >> "$CMDS"
done
echo "共 $(wc -l < "$CMDS") 个仿真（并发 $MAX_PAR）"
xargs -a "$CMDS" -P "$MAX_PAR" -L 1 bash "$HERE/../common/run_sim.sh"

CSV="$HERE/results/results.csv"
{
    echo -n "channels,size_mb,"
    python3 "$HERE/../common/parse_stats.py" --csv-header
} > "$CSV"
for NCH in 1 2 4 8 16; do
    LOG="$RUNS/ch${NCH}_${SIZE_MB}mb/run.log"
    [ -s "$LOG" ] || { echo "  !! 缺少 $LOG"; continue; }
    python3 "$HERE/../common/parse_stats.py" --csv "$LOG" "$NCH" "$SIZE_MB" >> "$CSV"
done
echo "结果: $CSV"
echo "06_channel_scale 完成"
