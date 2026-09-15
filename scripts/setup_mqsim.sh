#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
MQ="$ROOT/experiments/25_t02_t03_comparison/vendor/MQSim"
REV=51f0f2d3fed92d88ef4a0fa61a38024b07bf9d16
if [[ ! -e "$MQ" ]]; then
  git clone https://github.com/CMU-SAFARI/MQSim.git "$MQ"
  git -C "$MQ" checkout --detach "$REV"
fi
[[ "$(git -C "$MQ" rev-parse HEAD)" == "$REV" ]] || { echo 'MQSim revision mismatch' >&2; exit 1; }
git -C "$MQ" diff --quiet HEAD || { echo 'MQSim has modified sources' >&2; exit 1; }
make -C "$MQ" -j"${JOBS:-4}"
