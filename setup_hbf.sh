#!/bin/bash
# Install HBF module into the gpgpu-sim source tree.
# Run this AFTER `source ./gpu-simulator/setup_environment.sh`
# which clones gpgpu-sim into gpu-simulator/gpgpu-sim/.

set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
GSIM="$SCRIPT_DIR/gpu-simulator/gpgpu-sim"

if [ ! -d "$GSIM/src/gpgpu-sim" ]; then
    echo "Error: gpgpu-sim not found at $GSIM"
    echo "Run 'source ./gpu-simulator/setup_environment.sh' first."
    exit 1
fi

echo "Installing HBF module..."

# 1. Copy new HBF source files
cp "$SCRIPT_DIR/hbf/hbf.h" "$GSIM/src/gpgpu-sim/"
cp "$SCRIPT_DIR/hbf/hbf.cc" "$GSIM/src/gpgpu-sim/"

# 2. Apply patches to existing files
cd "$GSIM"
for patch in "$SCRIPT_DIR/hbf/patches/"*.patch; do
    echo "  Applying $(basename "$patch")..."
    git apply --ignore-whitespace "$patch" 2>/dev/null || {
        echo "  Warning: git apply failed for $(basename "$patch"), trying patch command..."
        patch -p1 < "$patch" 2>/dev/null || echo "  Warning: patch also failed, file may already be patched"
    }
done

# 3. Copy HBF config
cp "$SCRIPT_DIR/hbf/gpgpusim_hbf.config" \
   "$GSIM/configs/tested-cfgs/SM7_QV100/"

echo "HBF module installed."
echo "Now run: make -j\$(nproc) -C ./gpu-simulator"
