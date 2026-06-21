#!/bin/bash
# Quick smoke test: compile CUDA vectorAdd and run through GPGPU-Sim PTX mode
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

# Setup environment
export CUDA_INSTALL_PATH=/usr/local/cuda
source ./gpu-simulator/setup_environment.sh 2>/dev/null

# Compile the test program with dynamic CUDA runtime
echo "=== Compiling vectorAdd ==="
nvcc -arch=sm_70 --cudart shared -o vectorAdd vectorAdd.cu

echo ""
echo "=== Running vectorAdd through GPGPU-Sim PTX mode ==="
echo "Using GPGPU-Sim config: $GPGPUSIM_ROOT/configs/tested-cfgs/SM7_QV100/gpgpusim.config"
echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo ""

# Link gpgpusim.config into current dir (GPGPU-Sim looks for it in CWD)
ln -sf "$GPGPUSIM_ROOT/configs/tested-cfgs/SM7_QV100/gpgpusim.config" gpgpusim.config
ln -sf "$GPGPUSIM_ROOT/configs/tested-cfgs/SM7_QV100/config_volta_islip.icnt" config_volta_islip.icnt 2>/dev/null || true

# Run the test - GPGPU-Sim intercepts CUDA calls via libcudart.so
./vectorAdd

echo ""
echo "=== Smoke test complete ==="
