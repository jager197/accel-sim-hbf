#!/bin/bash
# ============================================================================
# 03_weight_load — LLM 权重 / 预填充顺序加载实验
#
# 场景: 推理初始化 / prefill 阶段把权重从近存储层顺序读入 SM。
# 对比 DRAM 与 HBF 的有效带宽:
#   - DRAM: 每个 128 B 请求走 HBM 通道（dram_latency=100, 16B/cycle 总线）
#   - HBF: 同一 4 KB page 的 32 个请求被 MSHR 合并为一次 tR=15K 的页读，
#          8 分区 × max_active=64 个子阵列并行。
#   理论聚合上限 = 8 分区 × 64 × 4KB / tR × 1.132GHz ≈ 158 GB/s；
#   实测受 GPGPU-Sim 前端（SM 发射/在飞请求数）限制，见 results.md 注释。
#
# 修复说明（本次重构）:
#   - 地址映射 dramid@8 → dramid@12: 4KB 页级交错，同一 NAND 页只归一个
#     分区（旧 256B 交错让同一页被 8 个分区各读一遍，8 倍浪费）。
#   - weight_load 8 路展开（先发全部 load 再累加）+ 1024 warps: 每 warp
#     8 个读在飞，喂满 HBF 子阵列。
#   - 保留 limited_hbf_oldmap 对照: 16MB 修复前后对比。
#
# 用法: bash experiments/03_weight_load/run.sh [SIZE_LIST_MB]
# 产物: results/results.csv, results/results.md, results/bandwidth_plot.png
# ============================================================================
set -u
source "$(dirname "$0")/../common/start_logging.sh" "$0"
HERE="$(cd "$(dirname "$0")" && pwd)"
source "$HERE/../common/env.sh"

SIZE_LIST="${1:-1,4,8,16}"   # MB
MAX_PAR="${MAX_PAR:-8}"
RUN_TIMEOUT="${RUN_TIMEOUT:-5400}"   # 大尺寸 HBF 运行慢，放宽超时

RUNS="$HERE/runs"
BIN="$HERE/../workloads_bin"
mkdir -p "$RUNS" "$BIN" "$HERE/results"
source "$HERE/../configs/gen_configs.sh" >/dev/null

nvcc $SM70 --cudart shared -o "$BIN/weight_load" "$HERE/../workloads/weight_load.cu"
echo "weight_load 已编译"

# ═══ 03 本地 fast 配置变体 ═══
# 放大 L2→存储层的仲裁信用（信用 = frfcfs + returnq - 1）:
# 默认 frfcfs=64/returnq=192 → 信用 ≈ 255/分区 → 全局 ~2K 在飞请求，
# 喂不满 HBF 的 512 个页读槽位（需 ~65K 在飞）。放大到 4096+4096 → 信用
# ≈ 8191/分区 ≈ 65K 全局。实测带宽 5.6 → 13.7 GB/s（16MB）。
# 注意: frfcfs 不能为 0（DRAM FR-FCFS 调度器断言），故用大数值。
# DRAM 与 HBF 两侧同配置，保证层级对比公平。
for V in "unlimited_fast:unlimited" "limited_hbf_fast:limited_hbf" \
         "limited_hbf_oldmap_fast:limited_hbf_oldmap"; do
    name="${V%%:*}"; base="${V#*:}"
    sed -e 's/-gpgpu_frfcfs_dram_sched_queue_size .*/-gpgpu_frfcfs_dram_sched_queue_size 4096/' \
        -e 's/-gpgpu_dram_return_queue_size .*/-gpgpu_dram_return_queue_size 4096/' \
        "$CFG_DIR/$base.config" > "$RUNS/$name.config"
    echo "  $name.config (信用放大)"
done

# 两个层级的配置映射: dram → unlimited_fast, hbf → limited_hbf_fast
CMDS="$RUNS/cmds.txt"; : > "$CMDS"
for MB in $(echo "$SIZE_LIST" | tr ',' ' '); do
    echo "$RUNS/dram_${MB}mb  $RUNS/unlimited_fast.config       $BIN/weight_load $MB 0 $HBF_BASE" >> "$CMDS"
    echo "$RUNS/hbf_${MB}mb   $RUNS/limited_hbf_fast.config     $BIN/weight_load $MB 1 $HBF_BASE" >> "$CMDS"
done
# 修复前后对照: 16MB 用旧 256B 交错映射（同一页被 8 个分区重复读），同信用解除
echo "$RUNS/hbf_16mb_oldmap $RUNS/limited_hbf_oldmap_fast.config $BIN/weight_load 16 1 $HBF_BASE" >> "$CMDS"
echo "共 $(wc -l < "$CMDS") 个仿真（并发 $MAX_PAR）"
xargs -a "$CMDS" -P "$MAX_PAR" -L 1 bash "$HERE/../common/run_sim.sh"

CSV="$HERE/results/results.csv"
{
    echo -n "tier,size_mb,entries,"
    python3 "$HERE/../common/parse_stats.py" --csv-header
} > "$CSV"
for MB in $(echo "$SIZE_LIST" | tr ',' ' '); do
    for TIER in dram hbf; do
        LOG="$RUNS/${TIER}_${MB}mb/run.log"
        [ -s "$LOG" ] || { echo "  !! 缺少 $LOG"; continue; }
        python3 "$HERE/../common/parse_stats.py" --csv "$LOG" \
            "$TIER" "$MB" "$((MB * 8192))" >> "$CSV"
    done
done
python3 "$HERE/../common/parse_stats.py" --csv "$RUNS/hbf_16mb_oldmap/run.log" \
    "hbf_oldmap" 16 131072 >> "$CSV"
echo "结果: $CSV"

python3 "$HERE/plot.py" "$CSV" "$HERE/results"
echo "03_weight_load 完成，产物见 $HERE/results/"
