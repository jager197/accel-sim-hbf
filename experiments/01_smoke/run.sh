#!/bin/bash
# ============================================================================
# 01_smoke — 模拟器可用性验证
#
# 回答"模拟器能不能用":
#   S1  构建与工具链就绪
#   S2  DRAM 路径功能正确性（vector_add: sin²+cos²=1）
#   S3  HBF 地址路由 + 读路径（请求确实到达 HBF 控制器）
#   S4  HBF 各子系统统计健全（MSHR/页缓存/FTL/GC/磨损/容量/子阵列分布）
#   S5  HBF 写路径（写缓冲→擦除→编程）无死锁完成
#   S6  顺序流 MSHR 合并率合理（weight_load 小规模）
#
# 产物: runs/<name>/run.log + results/smoke_report.txt
# 运行: bash experiments/01_smoke/run.sh        （全量）
#       RUN_SLOW=0 bash experiments/01_smoke/run.sh  （跳过耗时的写路径验证）
# ============================================================================
set -u
source "$(dirname "$0")/../common/start_logging.sh" "$0"
HERE="$(cd "$(dirname "$0")" && pwd)"
source "$HERE/../common/env.sh"
RUN_SLOW="${RUN_SLOW:-1}"

WORK="$HERE/workloads_bin"
RUNS="$HERE/runs"
mkdir -p "$WORK" "$RUNS" "$HERE/results"
source "$HERE/../configs/gen_configs.sh" >/dev/null

PASS=0; FAIL=0
check() {  # check <name> <log> <grep_pattern>
    if grep -qE "$3" "$2"; then
        echo "  [PASS] $1"; PASS=$((PASS+1))
    else
        echo "  [FAIL] $1  (missing: $3)"; FAIL=$((FAIL+1))
    fi
}

echo "==== S1: 环境与工具链 ===="
if command -v nvcc >/dev/null; then
    echo "  [PASS] nvcc: $($CUDA_INSTALL_PATH/bin/nvcc --version 2>/dev/null | grep release | head -1)"
    PASS=$((PASS+1))
else
    echo "  [FAIL] nvcc 不存在"
    FAIL=$((FAIL+1))
fi
if ls "$GPGPUSIM_ROOT/lib"/gcc-*/cuda-*/release/libcudart.so.* >/dev/null 2>&1; then
    echo "  [PASS] GPGPU-Sim libcudart 已编译"
    PASS=$((PASS+1))
else
    echo "  [FAIL] GPGPU-Sim libcudart 未找到（先 source gpu-simulator/setup_environment.sh 并编译）"
    FAIL=$((FAIL+1))
fi

echo "==== S2: 编译负载 ===="
nvcc $SM70 --cudart shared -o "$WORK/vector_add"  "$HERE/../workloads/vector_add.cu"  && echo "  [PASS] vector_add"
nvcc $SM70 --cudart shared -o "$WORK/hbf_access"  "$HERE/../workloads/hbf_access.cu"  && echo "  [PASS] hbf_access"
nvcc $SM70 --cudart shared -o "$WORK/weight_load" "$HERE/../workloads/weight_load.cu" && echo "  [PASS] weight_load"

echo "==== S3: DRAM 功能正确性（vector_add, 纯 DRAM 配置） ===="
bash "$HERE/../common/run_sim.sh" "$RUNS/vector_add" "$CFG_DIR/unlimited.config" "$WORK/vector_add"
check "vectorAdd 数值正确" "$RUNS/vector_add/run.log" "Test PASSED"

echo "==== S4: HBF 路由 + 读路径 + 子系统统计（hbf_access, HBF 配置） ===="
bash "$HERE/../common/run_sim.sh" "$RUNS/hbf_access_read" "$CFG_DIR/limited_hbf.config" \
    "$WORK/hbf_access" "$HBF_BASE" 0
L="$RUNS/hbf_access_read/run.log"
check "DRAM 路径仍正确"        "$L" "\[DRAM Test\] PASSED"
check "HBF 读完成无死锁"       "$L" "\[HBF-READ\] completed"
check "请求到达 HBF 控制器"    "$L" "HBF Total Requests:\s+[1-9]"
check "MSHR 合并发生"          "$L" "HBF MSHR Hits:\s+[1-9]"
check "NAND 页读发生"          "$L" "HBF Page Reads:\s+[1-9]"
check "页缓存统计输出"         "$L" "HBF Page Cache:.*enabled"
check "FTL 映射生效"           "$L" "HBF FTL Translations:\s+[1-9]"
check "容量使用率统计输出"     "$L" "HBF Capacity Usage:"
check "子阵列分布统计输出"     "$L" "HBF Subarray Spread:"
check "FTL 块统计输出"         "$L" "HBF FTL Total Blocks:"

echo "==== S5: HBF 写路径（写缓冲→擦除→编程→读回校验，慢，可 RUN_SLOW=0 跳过） ===="
if [ "$RUN_SLOW" = "1" ]; then
    bash "$HERE/../common/run_sim.sh" "$RUNS/hbf_access_rw" "$CFG_DIR/limited_hbf.config" \
        "$WORK/hbf_access" "$HBF_BASE" 1
    L="$RUNS/hbf_access_rw/run.log"
    check "写读往返数据正确 (errors=0)" "$L" "\[HBF-RW\] PASSED"
    check "NAND 页编程发生"    "$L" "HBF Page Programs:\s+[1-9]"
    check "块擦除发生"         "$L" "HBF Block Erases:\s+[1-9]"
    check "磨损统计输出"       "$L" "HBF FTL Erase Count:"
else
    echo "  [SKIP] RUN_SLOW=0"
fi

echo "==== S6: 顺序流 MSHR 合并（weight_load 4MB, HBF） ===="
bash "$HERE/../common/run_sim.sh" "$RUNS/weight_load_4mb" "$CFG_DIR/limited_hbf.config" \
    "$WORK/weight_load" 4 1 "$HBF_BASE"
L="$RUNS/weight_load_4mb/run.log"
check "顺序流完成"            "$L" "\[WEIGHT-LOAD\].*errors=0"
RATE=$(python3 "$HERE/../common/parse_stats.py" "$L" \
    | awk -F= '/^mshr_hits=/{h=$2} /^hbf_requests=/{r=$2} END{if(r>0) printf "%.1f", 100*h/r; else print "NA"}')
echo "  聚合 MSHR 合并率: ${RATE}%"
if [ "$RATE" != "NA" ] && awk "BEGIN{exit !($RATE >= 80)}"; then
    echo "  [PASS] MSHR 合并率 >= 80%"
    PASS=$((PASS+1))
else
    echo "  [FAIL] MSHR 合并率 < 80% (${RATE}%)"
    FAIL=$((FAIL+1))
fi

echo ""
echo "========== SMOKE VERDICT: $PASS passed, $FAIL failed =========="
{
    echo "# HBF-Sim 冒烟测试报告"
    echo ""
    echo "- 生成时间: $(date '+%Y-%m-%d %H:%M:%S')"
    echo "- 结果: $PASS passed, $FAIL failed"
    echo "- 运行目录: $RUNS"
    echo ""
    echo "覆盖: DRAM 功能正确性、HBF 路由/读路径、写→读回数据往返、"
    echo "MSHR/页缓存/FTL/GC/磨损/容量统计健全性。详见各 run.log。"
} > "$HERE/results/smoke_report.txt"
echo "报告: $HERE/results/smoke_report.txt"
[ "$FAIL" -eq 0 ]
