#!/bin/bash
# ============================================================================
# 09_sensitivity_profiles — E5: 跨时序档位稳健性（论文 §4.4）
#
# 问题: OCP v0.7.0 把 NAND 时序留给产品档位；论文结论必须在合理参数区间
# 内定性稳健。E5 把 E3 的判别性实验（覆盖写 2048 条目，hbf vs ssd 模式）
# 在三个时序档位下重跑:
#   aggressive — tR 15us / tPROG 200us / tBERS 2ms（默认 HBF 档）
#   cons       — tR 20us / tPROG 500us / tBERS 3ms（保守 HBF 档）
#   mqsim      — tR 75us / tPROG 750us / tBERS 3.8ms（MQSim 商品 NAND 档）
#
# 接受标准: "SSD 语义凭空产生 GC/搬移流量，HBF 模式为 0" 的定性结论
# 在三个档位下都成立。
#
# 用法: bash experiments/09_sensitivity_profiles/run.sh
# 产物: results/results.csv
# ============================================================================
set -u
source "$(dirname "$0")/../common/start_logging.sh" "$0"
HERE="$(cd "$(dirname "$0")" && pwd)"
source "$HERE/../common/env.sh"

ENTRIES="${1:-2048}"
MAX_PAR="${MAX_PAR:-4}"
RUN_TIMEOUT="${RUN_TIMEOUT:-5400}"

RUNS="$HERE/runs"
BIN="$HERE/../workloads_bin"
mkdir -p "$RUNS" "$BIN" "$HERE/results"
source "$HERE/../configs/gen_configs.sh" >/dev/null

nvcc $SM70 --cudart shared -o "$BIN/hbf_overwrite" "$HERE/../workloads/hbf_overwrite.cu"

CMDS="$RUNS/cmds.txt"; : > "$CMDS"
for PROF in aggressive cons mqsim; do
    for MODE in hbf ssd; do
        if [ "$PROF" = aggressive ]; then
            if [ "$MODE" = hbf ]; then
                CFG="$CFG_DIR/limited_hbf_smallblk.config"
            else
                CFG="$CFG_DIR/hbf_ssd_mode_smallblk.config"
            fi
        else
            CFG="$CFG_DIR/${MODE}_${PROF}_smallblk.config"
        fi
        echo "$RUNS/${PROF}_${MODE}  $CFG  $BIN/hbf_overwrite $ENTRIES $HBF_BASE" >> "$CMDS"
    done
done
echo "共 $(wc -l < "$CMDS") 个仿真（并发 $MAX_PAR）"
xargs -a "$CMDS" -P "$MAX_PAR" -L 1 bash "$HERE/../common/run_sim.sh"

CSV="$HERE/results/results.csv"
{
    echo -n "profile,mode,"
    python3 "$HERE/../common/parse_stats.py" --csv-header
} > "$CSV"
for PROF in aggressive cons mqsim; do
    for MODE in hbf ssd; do
        LOG="$RUNS/${PROF}_${MODE}/run.log"
        [ -s "$LOG" ] || { echo "  !! 缺少 $LOG"; continue; }
        python3 "$HERE/../common/parse_stats.py" --csv "$LOG" "$PROF" "$MODE" >> "$CSV"
    done
done
echo "结果: $CSV"
echo "09_sensitivity_profiles 完成"
