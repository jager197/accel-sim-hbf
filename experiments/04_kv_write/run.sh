#!/bin/bash
# ============================================================================
# 04_kv_write — KV cache 追加写实验（新增）
#
# 场景: decode 阶段每个新 token 的 K/V 追加写入 KV cache。
# 对比 DRAM 与 HBF 的写路径:
#   - DRAM: 写请求直接进 HBM 通道
#   - HBF: 写缓冲把同 page 的 32 条写合并为一次 NAND 页编程；
#          新块首次写触发块擦除（erase-before-write）
#
# 观察点:
#   - 每条目写成本（cycles/entry）随规模的变化
#   - HBF 写合并效率（1 次编程吞下多少条 128B 写）
#   - 块擦除的固定开销如何在批量写中摊薄
#
# 用法: bash experiments/04_kv_write/run.sh [ENTRY_LIST] [MAX_PAR]
# 产物: results/results.csv, results/results.md, results/write_plot.png
# ============================================================================
set -u
source "$(dirname "$0")/../common/start_logging.sh" "$0"
HERE="$(cd "$(dirname "$0")" && pwd)"
source "$HERE/../common/env.sh"

ENTRY_LIST="${1:-64,256,1024,4096}"
MAX_PAR="${MAX_PAR:-8}"

RUNS="$HERE/runs"
BIN="$HERE/../workloads_bin"
mkdir -p "$RUNS" "$BIN" "$HERE/results"
source "$HERE/../configs/gen_configs.sh" >/dev/null

nvcc $SM70 --cudart shared -o "$BIN/kv_write" "$HERE/../workloads/kv_write.cu"
echo "kv_write 已编译"

CMDS="$RUNS/cmds.txt"; : > "$CMDS"
for W in $(echo "$ENTRY_LIST" | tr ',' ' '); do
    echo "$RUNS/dram_${W}e  $CFG_DIR/unlimited.config   $BIN/kv_write $W 0 $HBF_BASE" >> "$CMDS"
    echo "$RUNS/hbf_${W}e   $CFG_DIR/limited_hbf.config $BIN/kv_write $W 1 $HBF_BASE" >> "$CMDS"
done
echo "共 $(wc -l < "$CMDS") 个仿真（并发 $MAX_PAR）"
xargs -a "$CMDS" -P "$MAX_PAR" -L 1 bash "$HERE/../common/run_sim.sh"

CSV="$HERE/results/results.csv"
{
    echo -n "tier,entries,"
    python3 "$HERE/../common/parse_stats.py" --csv-header
} > "$CSV"
for W in $(echo "$ENTRY_LIST" | tr ',' ' '); do
    for TIER in dram hbf; do
        LOG="$RUNS/${TIER}_${W}e/run.log"
        [ -s "$LOG" ] || { echo "  !! 缺少 $LOG"; continue; }
        python3 "$HERE/../common/parse_stats.py" --csv "$LOG" \
            "$TIER" "$W" >> "$CSV"
    done
done
echo "结果: $CSV"

python3 "$HERE/plot.py" "$CSV" "$HERE/results"
echo "04_kv_write 完成，产物见 $HERE/results/"
