#!/bin/bash
# Install HBF module into the gpgpu-sim source tree (v0.4).
# Run this AFTER `source ./gpu-simulator/setup_environment.sh`
# which clones gpgpu-sim into gpu-simulator/gpgpu-sim/.
#
# v0.4: copies ALL hbf module sources, installs the base config, and applies
# the consolidated integration patch (hbf/patches/v0.4-all.patch), which
# carries every gpgpu-sim integration change (config fields, option
# registration, L2 routing, 64-bit address plumbing, ...) relative to the
# upstream gpgpu-sim HEAD.

set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
GSIM="$SCRIPT_DIR/gpu-simulator/gpgpu-sim"

if [ ! -d "$GSIM/src/gpgpu-sim" ]; then
    echo "Error: gpgpu-sim not found at $GSIM"
    echo "Run 'source ./gpu-simulator/setup_environment.sh' first."
    exit 1
fi

echo "Installing HBF module (v0.4)..."

# 1. Copy all HBF module sources (headers + implementations)
for f in "$SCRIPT_DIR/hbf/"*.h "$SCRIPT_DIR/hbf/"*.cc; do
    [ -f "$f" ] || continue
    echo "  cp $(basename "$f")"
    cp "$f" "$GSIM/src/gpgpu-sim/"
done

# 2. Apply patches to existing files
cd "$GSIM"
for patch in "$SCRIPT_DIR/hbf/patches/"*.patch; do
    echo "  Applying $(basename "$patch")..."
    git apply --ignore-whitespace "$patch" 2>/dev/null || {
        echo "  Warning: git apply failed for $(basename "$patch"), trying patch command..."
        patch -p1 < "$patch" 2>/dev/null || echo "  Warning: patch also failed, file may already be patched"
    }
done

# 3. Install the HBF base config
mkdir -p "$GSIM/configs/tested-cfgs/SM7_QV100"
cp "$SCRIPT_DIR/hbf/gpgpusim_hbf.config" \
   "$GSIM/configs/tested-cfgs/SM7_QV100/"

echo "HBF module installed."
echo "Now run: make -j\$(nproc) -C ./gpu-simulator"
