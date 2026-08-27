#!/bin/bash
# ============================================================================
# 02_kv_cache — KV cache 规模扩展实验（核心收益实验）
#
# 场景: 自回归 LLM 推理的注意力读取。DRAM 容量固定为 D 条 KV 条目，
#       超出部分溢出到第二层存储。对比三种内存层级配置:
#         unlimited    纯 DRAM（无限容量基准）
#         limited      DRAM + 外部慢速溢出（串行/无合并/无缓存）
#         limited_hbf  DRAM + HBF（MSHR 合并 + 页缓存 + 子阵列并行）
#
# 扩展点（相对旧实验 N∈{8,16,24}）:
#   - 规模扫描: N ∈ {8,16,24,32,48,64} → 溢出率 0%–87.5%
#   - window 扫描: N=32, window ∈ {2,4,8,16} → 展示页缓存复用收益
#
# 用法: bash experiments/02_kv_cache/run.sh [N_LIST] [D] [WINDOW] [MAX_PAR]
# 产物: results/results.csv, results/results.md, results/*.png
# ============================================================================
set -u
source "$(dirname "$0")/../common/start_logging.sh" "$0"
HERE="$(cd "$(dirname "$0")" && pwd)"
source "$HERE/../common/env.sh"

N_LIST="${1:-8,16,24,32,48,64}"
D="${2:-8}"
WINDOW="${3:-4}"
MAX_PAR="${MAX_PAR:-12}"

RUNS="$HERE/runs"
BIN="$HERE/../workloads_bin"
mkdir -p "$RUNS" "$BIN" "$HERE/results"
source "$HERE/../configs/gen_configs.sh" >/dev/null

# 编译负载（幂等）
nvcc $SM70 --cudart shared -o "$BIN/kv_swa" "$HERE/../workloads/kv_swa.cu"
echo "kv_swa 已编译: $BIN/kv_swa"

# ---- 收集全部 (case,N) 运行命令 ----
CMDS="$RUNS/cmds.txt"; : > "$CMDS"
for CASE in unlimited limited limited_hbf; do
    for N in $(echo "$N_LIST" | tr ',' ' '); do
        echo "$RUNS/${CASE}_N${N} $CFG_DIR/$CASE.config $BIN/kv_swa $N $D $WINDOW $HBF_BASE" >> "$CMDS"
    done
done
echo "主扫描: $(wc -l < "$CMDS") 个仿真（并发 $MAX_PAR）"
xargs -a "$CMDS" -P "$MAX_PAR" -L 1 bash "$HERE/../common/run_sim.sh"

# ---- 提取主扫描结果 ----
CSV="$HERE/results/results.csv"
{
    echo -n "case,N,D,window,overflow_pct,"
    python3 "$HERE/../common/parse_stats.py" --csv-header
} > "$CSV"
for CASE in unlimited limited limited_hbf; do
    for N in $(echo "$N_LIST" | tr ',' ' '); do
        LOG="$RUNS/${CASE}_N${N}/run.log"
        [ -s "$LOG" ] || { echo "  !! 缺少 $LOG"; continue; }
        OVERFLOW=$(awk "BEGIN{printf \"%.1f\", ($N-$D)*100.0/$N}")
        python3 "$HERE/../common/parse_stats.py" --csv "$LOG" \
            "$CASE" "$N" "$D" "$WINDOW" "$OVERFLOW" >> "$CSV"
    done
done
echo "主扫描结果: $CSV"

# ---- window 扫描（limited_hbf, N=32） ----
WCSV="$HERE/results/window_sweep.csv"
{
    echo -n "case,N,D,window,overflow_pct,"
    python3 "$HERE/../common/parse_stats.py" --csv-header
} > "$WCSV"
for W in 2 4 8 16; do
    RDIR="$RUNS/hbf_window_${W}"
    bash "$HERE/../common/run_sim.sh" "$RDIR" "$CFG_DIR/limited_hbf.config" \
        "$BIN/kv_swa" 32 "$D" "$W" "$HBF_BASE"
    OVERFLOW=$(awk "BEGIN{printf \"%.1f\", (32-$D)*100.0/32}")
    python3 "$HERE/../common/parse_stats.py" --csv "$RDIR/run.log" \
        "limited_hbf" 32 "$D" "$W" "$OVERFLOW" >> "$WCSV"
done
echo "window 扫描结果: $WCSV"

# ---- 图表与汇总 ----
python3 "$HERE/plot.py" "$CSV" "$WCSV" "$HERE/results"
echo "02_kv_cache 完成，产物见 $HERE/results/"
