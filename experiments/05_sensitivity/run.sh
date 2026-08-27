#!/bin/bash
# ============================================================================
# 05_sensitivity — 机制消融与参数敏感性
#
# A. 机制消融（kv_swa N=32 D=8 W=4）:
#    关 MSHR / 关页缓存 / 关页缓冲 / 串行化 —— 有复用负载里页缓存吸收
#    大量重复读，差异主要体现在页读数/合并数上。
# B. 参数扫描（选能体现参数效应的负载）:
#    - 页缓存容量 {0,64,256,1024} @ kv_swa N=32
#    - tR {10K,20K,40K} @ 外溢层级（无缓存/无合并/串行 → 时延线性响应）
#    - max_active {8,64,256} @ weight_load 4MB（无复用流式，带宽 ∝ max_active）
#
# 用法: bash experiments/05_sensitivity/run.sh
# 产物: results/ablation.csv, results/sweeps.csv, results/*.png, results/results.md
# ============================================================================
set -u
source "$(dirname "$0")/../common/start_logging.sh" "$0"
HERE="$(cd "$(dirname "$0")" && pwd)"
source "$HERE/../common/env.sh"

MAX_PAR="${MAX_PAR:-8}"
N=32; D=8; W=4        # 消融/缓存扫描负载参数
T_N=24                # tR 扫描用 kv 规模（溢出 67%）
WL_MB=4               # max_active 扫描用 weight_load 规模

RUNS="$HERE/runs"
BIN="$HERE/../workloads_bin"
mkdir -p "$RUNS" "$BIN" "$HERE/results"
source "$HERE/../configs/gen_configs.sh" >/dev/null

nvcc $SM70 --cudart shared -o "$BIN/kv_swa" "$HERE/../workloads/kv_swa.cu"
nvcc $SM70 --cudart shared -o "$BIN/weight_load" "$HERE/../workloads/weight_load.cu"

# 单参数变体生成（注意生成配置行首有前导空格，不能用 ^ 锚定）
variant() {  # variant <base_config> <name> <sed-args...>
    local base="$1"; local name="$2"; shift 2
    sed "$@" "$base" > "$RUNS/$name.config"
}

# 缓存容量（基于 limited_hbf）
for V in "cache64:s/-gpgpu_hbf_cache_entries .*/ -gpgpu_hbf_cache_entries 64/" \
         "cache1024:s/-gpgpu_hbf_cache_entries .*/ -gpgpu_hbf_cache_entries 1024/"; do
    name="${V%%:*}"; expr="${V#*:}"
    variant "$CFG_DIR/limited_hbf.config" "$name" -e "$expr"
done
# tR（基于外溢层级 limited：无缓存/无合并/串行，时延线性）
for V in "tR10k:s/-gpgpu_hbf_tR .*/ -gpgpu_hbf_tR 10000/" \
         "tR40k:s/-gpgpu_hbf_tR .*/ -gpgpu_hbf_tR 40000/"; do
    name="${V%%:*}"; expr="${V#*:}"
    variant "$CFG_DIR/limited.config" "$name" -e "$expr"
done
# max_active（基于 limited_hbf，weight_load 流式）
for V in "active8:s/-gpgpu_hbf_max_active .*/ -gpgpu_hbf_max_active 8/" \
         "active256:s/-gpgpu_hbf_max_active .*/ -gpgpu_hbf_max_active 256/"; do
    name="${V%%:*}"; expr="${V#*:}"
    variant "$CFG_DIR/limited_hbf.config" "$name" -e "$expr"
done

CMDS="$RUNS/cmds.txt"; : > "$CMDS"
# A. 消融（kv N=32）
for V in limited_hbf hbf_nomsgr hbf_nocache hbf_nobuffer hbf_serial; do
    echo "$RUNS/$V $CFG_DIR/$V.config $BIN/kv_swa $N $D $W $HBF_BASE" >> "$CMDS"
done
# B1. 缓存容量（kv N=32，0= hbf_nocache）
for V in hbf_nocache cache64 limited_hbf cache1024; do
    C="$CFG_DIR/$V.config"; [ -f "$C" ] || C="$RUNS/$V.config"
    echo "$RUNS/cache_$V $C $BIN/kv_swa $N $D $W $HBF_BASE" >> "$CMDS"
done
# B2. tR（外溢层级, kv N=24; 20K 基线 = limited）
for V in tR10k limited tR40k; do
    C="$CFG_DIR/$V.config"; [ -f "$C" ] || C="$RUNS/$V.config"
    echo "$RUNS/tr_$V $C $BIN/kv_swa $T_N $D $W $HBF_BASE" >> "$CMDS"
done
# B3. max_active（weight_load 4MB, blocks=128 → 1024 warps, 64 基线 = limited_hbf）
# 1024 warps 才能喂满 64 个子阵列（256 warps 时 issue-limited 掩盖并行度）
for V in active8 limited_hbf active256; do
    C="$CFG_DIR/$V.config"; [ -f "$C" ] || C="$RUNS/$V.config"
    echo "$RUNS/ma_$V $C $BIN/weight_load $WL_MB 1 $HBF_BASE 128" >> "$CMDS"
done
echo "共 $(wc -l < "$CMDS") 个仿真（并发 $MAX_PAR）"
xargs -a "$CMDS" -P "$MAX_PAR" -L 1 bash "$HERE/../common/run_sim.sh"

# ---- 提取 ----
HDR=$(python3 "$HERE/../common/parse_stats.py" --csv-header)
{
    echo "series,variant,$HDR"
} > "$HERE/results/ablation.csv"
for V in limited_hbf hbf_nomsgr hbf_nocache hbf_nobuffer hbf_serial; do
    python3 "$HERE/../common/parse_stats.py" --csv "$RUNS/$V/run.log" \
        "ablation" "$V" >> "$HERE/results/ablation.csv"
done

{
    echo "series,variant,$HDR"
} > "$HERE/results/sweeps.csv"
python3 "$HERE/../common/parse_stats.py" --csv "$RUNS/cache_hbf_nocache/run.log" "cache" 0     >> "$HERE/results/sweeps.csv"
python3 "$HERE/../common/parse_stats.py" --csv "$RUNS/cache_cache64/run.log"     "cache" 64    >> "$HERE/results/sweeps.csv"
python3 "$HERE/../common/parse_stats.py" --csv "$RUNS/cache_limited_hbf/run.log" "cache" 256   >> "$HERE/results/sweeps.csv"
python3 "$HERE/../common/parse_stats.py" --csv "$RUNS/cache_cache1024/run.log"   "cache" 1024  >> "$HERE/results/sweeps.csv"
python3 "$HERE/../common/parse_stats.py" --csv "$RUNS/tr_tR10k/run.log"          "tR" 10000    >> "$HERE/results/sweeps.csv"
python3 "$HERE/../common/parse_stats.py" --csv "$RUNS/tr_limited/run.log"        "tR" 20000    >> "$HERE/results/sweeps.csv"
python3 "$HERE/../common/parse_stats.py" --csv "$RUNS/tr_tR40k/run.log"          "tR" 40000    >> "$HERE/results/sweeps.csv"
python3 "$HERE/../common/parse_stats.py" --csv "$RUNS/ma_active8/run.log"        "active" 8    >> "$HERE/results/sweeps.csv"
python3 "$HERE/../common/parse_stats.py" --csv "$RUNS/ma_limited_hbf/run.log"    "active" 64   >> "$HERE/results/sweeps.csv"
python3 "$HERE/../common/parse_stats.py" --csv "$RUNS/ma_active256/run.log"      "active" 256  >> "$HERE/results/sweeps.csv"

python3 "$HERE/plot.py" "$HERE/results/ablation.csv" "$HERE/results/sweeps.csv" "$HERE/results"
echo "05_sensitivity 完成，产物见 $HERE/results/"
