#!/usr/bin/env bash
# Install the canonical HBF-Sim sources into a pinned GPGPU-Sim checkout.
# `make bootstrap` is the normal entry point. This script is intentionally
# deterministic and does not select a branch or rewrite an existing checkout.

set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck disable=SC1091
source "$ROOT/artifact/integration.env"
GSIM="$ROOT/gpu-simulator/gpgpu-sim"
PATCH="$ROOT/$GPGPUSIM_PATCH"
SOURCE_MANIFEST="$ROOT/artifact/hbf_sources.txt"
GENERATED_MANIFEST="$ROOT/artifact/generated_sources.txt"
FORCE=0

case "${1:-}" in
  "") ;;
  --force) FORCE=1 ;;
  *) echo "usage: $0 [--force]" >&2; exit 2 ;;
esac

fail() {
  echo "setup_hbf.sh: $*" >&2
  exit 1
}

[[ -d "$GSIM/.git" ]] || fail "gpgpu-sim not found at $GSIM; run 'make bootstrap'"
[[ -f "$PATCH" ]] || fail "active integration patch not found: $PATCH"
[[ -f "$SOURCE_MANIFEST" ]] || fail "source manifest not found: $SOURCE_MANIFEST"
[[ -f "$GENERATED_MANIFEST" ]] || fail "generated-source manifest not found: $GENERATED_MANIFEST"

head_commit="$(git -C "$GSIM" rev-parse HEAD)"
[[ "$head_commit" == "$GPGPUSIM_COMMIT" ]] ||
  fail "GPGPU-Sim HEAD is $head_commit; expected $GPGPUSIM_COMMIT"

mapfile -t active_sources < <(sed -e 's/[[:space:]]*#.*$//' -e '/^[[:space:]]*$/d' "$SOURCE_MANIFEST")
(( ${#active_sources[@]} > 0 )) || fail "source manifest is empty"
for name in "${active_sources[@]}"; do
  [[ "$name" != */* && "$name" =~ ^[A-Za-z0-9_.+-]+$ ]] ||
    fail "unsafe source-manifest entry: $name"
  [[ -f "$ROOT/hbf/$name" ]] || fail "missing canonical source: hbf/$name"
done
[[ -f "$ROOT/hbf/gpgpusim_hbf.config" ]] || fail "missing canonical HBF config"

mapfile -t generated_sources < <(sed -e 's/[[:space:]]*#.*$//' -e '/^[[:space:]]*$/d' "$GENERATED_MANIFEST")
(( ${#generated_sources[@]} > 0 )) || fail "generated-source manifest is empty"
for name in "${generated_sources[@]}"; do
  [[ "$name" =~ ^[A-Za-z0-9_.+-]+(/[A-Za-z0-9_.+-]+)*$ && "$name" != *".."* ]] ||
    fail "unsafe generated-source entry: $name"
  [[ -f "$ROOT/artifact/gpgpusim_generated/$name" ]] ||
    fail "missing generated parser snapshot: artifact/gpgpusim_generated/$name"
done

if git -C "$GSIM" apply --reverse --check "$PATCH" >/dev/null 2>&1; then
  echo "setup_hbf.sh: active integration patch already installed"
  actual_patch="$(mktemp)"
  trap 'rm -f "$actual_patch"' EXIT
  git -C "$GSIM" diff --binary --no-ext-diff > "$actual_patch"
  cmp -s "$PATCH" "$actual_patch" ||
    fail "installed integration diff contains changes outside the active patch"
elif git -C "$GSIM" apply --check "$PATCH" >/dev/null 2>&1; then
  if [[ -n "$(git -C "$GSIM" status --porcelain)" ]]; then
    fail "pinned checkout has local changes before installation; preserve them and use a clean checkout"
  fi
  echo "setup_hbf.sh: applying $(basename "$PATCH")"
  git -C "$GSIM" apply "$PATCH"
else
  fail "integration patch is neither cleanly applicable nor already installed"
fi

install -d "$GSIM/src/gpgpu-sim" "$GSIM/configs/tested-cfgs/SM7_QV100"
for name in "${active_sources[@]}"; do
  canonical="$ROOT/hbf/$name"
  installed="$GSIM/src/gpgpu-sim/$name"
  if [[ -e "$installed" ]] && ! cmp -s "$canonical" "$installed" && (( ! FORCE )); then
    fail "installed $name has local changes; preserve them or rerun with --force"
  fi
  install -m 0644 "$canonical" "$installed"
done
canonical_config="$ROOT/hbf/gpgpusim_hbf.config"
installed_config="$GSIM/configs/tested-cfgs/SM7_QV100/gpgpusim_hbf.config"
if [[ -e "$installed_config" ]] &&
   ! cmp -s "$canonical_config" "$installed_config" && (( ! FORCE )); then
  fail "installed HBF config has local changes; preserve it or rerun with --force"
fi
install -m 0644 "$canonical_config" "$installed_config"

# Install the parser snapshots used by the no-generator fallback.  They live
# outside the integration patch because they are generated inputs, not HBF
# source, and are verified by content below.
for name in "${generated_sources[@]}"; do
  generated_source="$ROOT/artifact/gpgpusim_generated/$name"
  generated_dest="$GSIM/$name"
  if [[ -e "$generated_dest" ]] && ! cmp -s "$generated_source" "$generated_dest" &&
     (( ! FORCE )); then
    fail "installed parser snapshot has local changes; preserve it or rerun with --force: $name"
  fi
  install -d "$(dirname "$generated_dest")"
  install -m 0644 "$generated_source" "$generated_dest"
done

"$ROOT/scripts/verify_install.sh"
echo "setup_hbf.sh: HBF-Sim installation complete"
