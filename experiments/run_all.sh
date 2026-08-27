#!/bin/bash
# ============================================================================
# experiments/run_all.sh — 一键运行全部实验
#
#   bash experiments/run_all.sh [MAX_PAR]
#
# 顺序: 01 冒烟(可用性) → 02 KV 规模(核心收益) → 03 权重加载 →
#       04 KV 写入 → 05 敏感性/消融。每步内部并发，跨步顺序执行
#       （避免抢占资源）。已完成且成功的 run 会被 run_sim.sh 缓存跳过，
#       中断后可重跑本脚本续跑。FORCE_RERUN=1 全量重跑。
#
# 产物: experiments/<NN_xx>/results/ + experiments/results/SUMMARY.md
# ============================================================================
set -u
source "$(dirname "$0")/common/start_logging.sh" "$0"
HERE="$(cd "$(dirname "$0")" && pwd)"
export MAX_PAR="${MAX_PAR:-${1:-12}}"
export RUN_TIMEOUT="${RUN_TIMEOUT:-3600}"

echo "=========================================================="
echo " HBF-Sim 实验套件 — 全部实验（MAX_PAR=$MAX_PAR）"
echo " 开始: $(date '+%Y-%m-%d %H:%M:%S')"
echo "=========================================================="

STEPS=(01_smoke 02_kv_cache 03_weight_load 04_kv_write 05_sensitivity)
OVERALL=0
for STEP in "${STEPS[@]}"; do
    echo ""
    echo "########## $STEP ##########"
    if bash "$HERE/$STEP/run.sh"; then
        echo "########## $STEP: OK ##########"
    else
        echo "########## $STEP: FAILED (rc=$?) — 继续后续步骤 ##########"
        OVERALL=1
    fi
done

# ---- 汇总（仅当尚不存在时生成索引；完整版见 SUMMARY.md 手动维护） ----
SUM="$HERE/results/SUMMARY.md"
if [ ! -f "$SUM" ]; then
{
    echo "# HBF-Sim 实验总览"
    echo ""
    echo "- 生成时间: $(date '+%Y-%m-%d %H:%M:%S')"
    echo "- 模拟器: accel-sim-hbf v0.3.1（GPGPU-Sim 4.2 + HBF 周期级控制器/FTL/页缓存）"
    echo ""
    echo "## 实验索引"
    echo ""
    echo "| 实验 | 问题 | 结果 |"
    echo "|---|---|---|"
    echo "| [01_smoke](01_smoke/results/smoke_report.txt) | 模拟器可用吗 | 功能正确性 + 子系统统计 PASS/FAIL |"
    echo "| [02_kv_cache](02_kv_cache/results/results.md) | HBF 对 KV 溢出的收益 | 规模/窗口扫描 + 加速比 |"
    echo "| [03_weight_load](03_weight_load/results/results.md) | HBF 顺序带宽 | DRAM vs HBF 有效带宽 |"
    echo "| [04_kv_write](04_kv_write/results/results.md) | HBF 写路径 | 写成本 + 合并效率 |"
    echo "| [05_sensitivity](05_sensitivity/results/results.md) | 收益来自哪个机制 | 消融 + 参数扫描 |"
    echo ""
} > "$SUM"
fi
echo "汇总: $SUM"
echo "全部实验结束: $(date '+%Y-%m-%d %H:%M:%S')"
exit $OVERALL
