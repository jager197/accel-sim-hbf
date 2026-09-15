#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
# shellcheck disable=SC1091
source "$ROOT/artifact/integration.env"
GSIM="$ROOT/gpu-simulator/gpgpu-sim"
REPO="${GPGPUSIM_REPO:-$GPGPUSIM_REPO_DEFAULT}"
CHECK_ONLY=0

case "${1:-}" in
  "") ;;
  --check-only) CHECK_ONLY=1 ;;
  *) echo "usage: $0 [--check-only]" >&2; exit 2 ;;
esac

for command_name in git install sed sha256sum; do
  command -v "$command_name" >/dev/null 2>&1 || {
    echo "bootstrap: missing required command: $command_name" >&2
    exit 1
  }
done

if (( CHECK_ONLY )); then
  exec "$ROOT/scripts/verify_install.sh"
fi

if [[ ! -d "$GSIM/.git" ]]; then
  if [[ -e "$GSIM" ]]; then
    echo "bootstrap: $GSIM exists but is not a Git checkout" >&2
    exit 1
  fi
  echo "bootstrap: cloning pinned GPGPU-Sim from $REPO"
  git clone --filter=blob:none --no-checkout "$REPO" "$GSIM"
  git -C "$GSIM" checkout --detach "$GPGPUSIM_COMMIT"
else
  head_commit="$(git -C "$GSIM" rev-parse HEAD)"
  if [[ "$head_commit" != "$GPGPUSIM_COMMIT" ]]; then
    echo "bootstrap: existing GPGPU-Sim is at $head_commit" >&2
    echo "bootstrap: expected $GPGPUSIM_COMMIT; existing work is never replaced" >&2
    exit 1
  fi
fi

"$ROOT/scripts/ensure_pybind11.sh"
"$ROOT/setup_hbf.sh"
echo "bootstrap: pinned installation is ready"
