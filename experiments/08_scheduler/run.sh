#!/bin/bash
# ============================================================================
# 08_scheduler — E4: 读写干扰与调度策略（论文 §6 Case Study）
#
# 对比三种 HBF 控制器调度策略在读写混合负载下的表现:
#   FCFS          — FIFO（默认）
#   read-priority — 读优先（写仅在无读时执行）
#   write-drain   — 读优先 + 写缓冲超过高水位时开启有界写窗口
#                   （窗口最长 hbf_write_drain_maxwait tick，防读饿死）
#
# 负载: hbf_mixed（n_read 个 warp 前台窗口读 + n_write 个 warp 后台追加写，
# 共享通道/子阵列，交错映射制造竞争）
# 变量: 调度策略 × 写压力 {8, 32, 64}（读固定 64 warp）
# 指标: 端到端周期、页读/编程次数、写窗口数、窗口内读延迟计数
#
# 用法: bash experiments/08_scheduler/run.sh
# 产物: results/results.csv, results/results.md
# ============================================================================
set -u
source "$(dirname "$0")/../common/start_logging.sh" "$0"
HERE="$(cd "$(dirname "$0")" && pwd)"
source "$HERE/../common/env.sh"

N_READ="${1:-64}"
WRITE_LIST="${2:-256,512,1024}"
TMAX="${3:-256}"
MAX_PAR="${MAX_PAR:-5}"
RUN_TIMEOUT="${RUN_TIMEOUT:-5400}"

RUNS="$HERE/runs"
BIN="$HERE/../workloads_bin"
mkdir -p "$RUNS" "$BIN" "$HERE/results"
source "$HERE/../configs/gen_configs.sh" >/dev/null

nvcc $SM70 --cudart shared -o "$BIN/hbf_mixed" "$HERE/../workloads/hbf_mixed.cu"

# 调度器配置变体（基于 limited_hbf 改调度策略 + 竞争参数）
# 竞争参数: 子阵列收窄到 32、并发槽降到 8 —— 页数（64 读页 + 写页）远超
# 槽位，MSHR 队列有真实排队，调度顺序才会影响完成时间。
for S in 0 1 2; do
    sed -e "s/-gpgpu_hbf_scheduler .*/-gpgpu_hbf_scheduler $S/" \
        -e 's/-gpgpu_hbf_num_subarrays .*/ -gpgpu_hbf_num_subarrays 32/' \
        -e 's/-gpgpu_hbf_max_active .*/ -gpgpu_hbf_max_active 8/' \
        -e 's/-gpgpu_hbf_cache_entries .*/ -gpgpu_hbf_cache_entries 0/' \
        -e 's/-gpgpu_hbf_write_drain_high .*/ -gpgpu_hbf_write_drain_high 1/' \
        "$CFG_DIR/limited_hbf.config" > "$RUNS/sched$S.config"
done

CMDS="$RUNS/cmds.txt"; : > "$CMDS"
for W in $(echo "$WRITE_LIST" | tr ',' ' '); do
    for S in 0 1 2; do
        echo "$RUNS/w${W}_s${S}  $RUNS/sched$S.config  $BIN/hbf_mixed $N_READ $W $TMAX $HBF_BASE" >> "$CMDS"
    done
done
echo "共 $(wc -l < "$CMDS") 个仿真（并发 $MAX_PAR）"
xargs -a "$CMDS" -P "$MAX_PAR" -L 1 bash "$HERE/../common/run_sim.sh"

CSV="$HERE/results/results.csv"
{
    echo -n "scheduler,nwrite,"
    python3 "$HERE/../common/parse_stats.py" --csv-header
} > "$CSV"
for W in $(echo "$WRITE_LIST" | tr ',' ' '); do
    for S in 0 1 2; do
        LOG="$RUNS/w${W}_s${S}/run.log"
        [ -s "$LOG" ] || { echo "  !! 缺少 $LOG"; continue; }
        python3 "$HERE/../common/parse_stats.py" --csv "$LOG" "$S" "$W" >> "$CSV"
    done
done
echo "结果: $CSV"
echo "08_scheduler 完成"
