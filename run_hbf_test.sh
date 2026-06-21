#!/bin/bash
# HBF Smoke Test — compile and run HBF test kernels through GPGPU-Sim PTX mode
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

# Setup environment
export CUDA_INSTALL_PATH=/usr/local/cuda
source ./gpu-simulator/setup_environment.sh 2>/dev/null

# Link HBF-enabled config
rm -f gpgpusim.config config_volta_islip.icnt
ln -sf "$GPGPUSIM_ROOT/configs/tested-cfgs/SM7_QV100/gpgpusim_hbf.config" gpgpusim.config
ln -sf "$GPGPUSIM_ROOT/configs/tested-cfgs/SM7_QV100/config_volta_islip.icnt" config_volta_islip.icnt 2>/dev/null || true

echo "=== Compiling HBF test ==="
nvcc -arch=sm_70 --cudart shared -o hbf_test hbf_test.cu

echo ""
echo "=== Running HBF test through GPGPU-Sim ==="
echo "Config: gpgpusim_hbf.config (HBF enabled, latency=10000, base=256GB, size=512GB)"
echo ""

./hbf_test

echo ""
echo "=== HBF smoke test complete ==="
echo "Check output above for HBF Controller statistics."
