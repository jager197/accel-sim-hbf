#!/bin/bash
# ============================================================================
# experiments/common/env.sh — 实验套件公共环境
#
# 用法：source "$(dirname "$0")/../common/env.sh"
#   （在任意实验脚本开头 source 一次即可）
#
# 提供：
#   CUDA_INSTALL_PATH    CUDA 工具链路径
#   GPGPUSIM_ROOT        gpgpu-sim 源码树（含已编译的 libcudart 拦截库）
#   REPO_ROOT            accel-sim-hbf 仓库根目录
#   EXP_ROOT             experiments/ 目录
#   CFG_DIR              experiments/configs（生成的配置目录）
#   ICXT_CFG             互连网络配置文件路径
#   HBF_BASE             HBF 地址段起始地址（必须与 gpgpusim.config 一致）
#   SM70                 编译架构参数
# ============================================================================

if [ -n "${_HBF_EXP_ENV_LOADED:-}" ]; then
    return 0 2>/dev/null || exit 0
fi

export CUDA_INSTALL_PATH="${CUDA_INSTALL_PATH:-/usr/local/cuda}"
# 上游 gpgpu-sim/setup_environment:75 导出的兼容开关：CUDA 12 构建下
# cudaRegisterFatBinary 走 extract_ptx_files 路径、不再填充
# cuobjdumpSectionList，若不设置该变量，pruneSectionList 得到空列表
# 直接 abort。任何非空值都能跳过该剪枝（本仓库编译于 CUDA 12.0）。
export CUOBJDUMP_SIM_FILE="${CUOBJDUMP_SIM_FILE:-jj}"

EXP_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REPO_ROOT="$(cd "$EXP_ROOT/.." && pwd)"

# 直接定位 GPGPU-Sim 树与编译产物，不 source 上游 setup_environment.sh：
# 该脚本在 set -u 下引用未定义变量会导致整个 shell 退出（实测），
# 而实验脚本普遍使用 set -u。
GPGPUSIM_ROOT="$REPO_ROOT/gpu-simulator/gpgpu-sim"
LIBDIR=$(ls -d "$GPGPUSIM_ROOT/lib"/gcc-*/cuda-*/release 2>/dev/null | head -1)
if [ -n "$LIBDIR" ]; then
    export LD_LIBRARY_PATH="$LIBDIR${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
fi
if [ ! -f "$GPGPUSIM_ROOT/configs/tested-cfgs/SM7_QV100/config_volta_islip.icnt" ]; then
    echo "env.sh: ERROR — GPGPU-Sim 树不完整，找不到 SM7_QV100 配置" >&2
    echo "        请先执行: source gpu-simulator/setup_environment.sh && bash setup_hbf.sh" >&2
    exit 1
fi
if [ ! -f "$LIBDIR/libcudart.so" ] 2>/dev/null; then
    echo "env.sh: ERROR — GPGPU-Sim 拦截库未编译: $LIBDIR" >&2
    echo "        请先执行: make -j\$(nproc) -C ./gpu-simulator" >&2
    exit 1
fi

CFG_DIR="$EXP_ROOT/configs"
ICXT_CFG="$GPGPUSIM_ROOT/configs/tested-cfgs/SM7_QV100/config_volta_islip.icnt"
HBF_BASE=274877906944   # 256 GB，与 gpgpusim_hbf.config 的 -gpgpu_hbf_base_addr 一致
SM70="-arch=sm_70"

# 导出给子进程（xargs/run_sim.sh 需要）
export CUDA_INSTALL_PATH EXP_ROOT REPO_ROOT GPGPUSIM_ROOT CFG_DIR ICXT_CFG HBF_BASE
export CUOBJDUMP_SIM_FILE
export _HBF_EXP_ENV_LOADED=1