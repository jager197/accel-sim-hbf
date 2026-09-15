#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
# shellcheck disable=SC1091
source "$ROOT/artifact/dependencies.env"
DEST="$ROOT/gpu-simulator/extern/pybind11"
REPO="${PYBIND11_REPO:-$PYBIND11_REPO_DEFAULT}"
CHECK_ONLY=0

case "${1:-}" in
  "") ;;
  --check-only) CHECK_ONLY=1 ;;
  *) echo "usage: $0 [--check-only]" >&2; exit 2 ;;
esac

command -v git >/dev/null 2>&1 || {
  echo "pybind11: git is required" >&2
  exit 1
}

if [[ ! -d "$DEST/.git" ]]; then
  if [[ -e "$DEST" ]]; then
    echo "pybind11: destination exists but is not a Git checkout: $DEST" >&2
    exit 1
  fi
  (( CHECK_ONLY == 0 )) || {
    echo "pybind11: pinned checkout is missing: $DEST" >&2
    exit 1
  }
  mkdir -p "$(dirname "$DEST")"
  git clone --filter=blob:none --no-checkout "$REPO" "$DEST"
  git -C "$DEST" fetch --depth=1 origin "$PYBIND11_COMMIT"
  git -C "$DEST" checkout --detach "$PYBIND11_COMMIT"
fi

actual="$(git -C "$DEST" rev-parse HEAD)"
[[ "$actual" == "$PYBIND11_COMMIT" ]] || {
  echo "pybind11: existing checkout is at $actual; expected $PYBIND11_COMMIT" >&2
  echo "pybind11: existing work is never replaced" >&2
  exit 1
}
[[ -z "$(git -C "$DEST" status --porcelain --untracked-files=all)" ]] || {
  echo "pybind11: pinned checkout is dirty: $DEST" >&2
  exit 1
}
echo "pybind11: pinned at $actual"
