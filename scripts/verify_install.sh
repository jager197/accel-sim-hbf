#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
# shellcheck disable=SC1091
source "$ROOT/artifact/integration.env"
GSIM="$ROOT/gpu-simulator/gpgpu-sim"
PATCH="$ROOT/$GPGPUSIM_PATCH"
SOURCE_MANIFEST="$ROOT/artifact/hbf_sources.txt"
GENERATED_MANIFEST="$ROOT/artifact/generated_sources.txt"
PYTHON="${PYTHON:-python3}"

fail() {
  echo "artifact-check: $*" >&2
  exit 1
}

[[ -d "$GSIM/.git" ]] || fail "missing GPGPU-Sim checkout: $GSIM"
[[ -f "$PATCH" ]] || fail "missing active integration patch: $PATCH"
[[ -f "$SOURCE_MANIFEST" ]] || fail "missing source manifest: $SOURCE_MANIFEST"
[[ -f "$GENERATED_MANIFEST" ]] || fail "missing generated-source manifest: $GENERATED_MANIFEST"
"$ROOT/scripts/ensure_pybind11.sh" --check-only

head_commit="$(git -C "$GSIM" rev-parse HEAD)"
[[ "$head_commit" == "$GPGPUSIM_COMMIT" ]] ||
  fail "GPGPU-Sim HEAD is $head_commit; expected $GPGPUSIM_COMMIT"

"$PYTHON" "$ROOT/scripts/verify_patch_install.py" \
  --repo "$GSIM" --commit "$GPGPUSIM_COMMIT" --patch "$PATCH" \
  --manifest "$SOURCE_MANIFEST" --config "$ROOT/hbf/gpgpusim_hbf.config" ||
  fail "installed tracked changes do not exactly match the active patch contract"

mapfile -t active_sources < <(sed -e 's/[[:space:]]*#.*$//' -e '/^[[:space:]]*$/d' "$SOURCE_MANIFEST")
(( ${#active_sources[@]} > 0 )) || fail "source manifest is empty"
for name in "${active_sources[@]}"; do
  [[ "$name" != */* && "$name" =~ ^[A-Za-z0-9_.+-]+$ ]] ||
    fail "unsafe source-manifest entry: $name"
  canonical="$ROOT/hbf/$name"
  installed="$GSIM/src/gpgpu-sim/$name"
  [[ -f "$canonical" ]] || fail "missing canonical source: hbf/$name"
  [[ -f "$installed" ]] || fail "missing installed source: src/gpgpu-sim/$name"
  cmp -s "$canonical" "$installed" || fail "installed $name differs from hbf/$name"
done

mapfile -t generated_sources < <(sed -e 's/[[:space:]]*#.*$//' -e '/^[[:space:]]*$/d' "$GENERATED_MANIFEST")
(( ${#generated_sources[@]} > 0 )) || fail "generated-source manifest is empty"
for name in "${generated_sources[@]}"; do
  [[ "$name" =~ ^[A-Za-z0-9_.+-]+(/[A-Za-z0-9_.+-]+)*$ && "$name" != *".."* ]] ||
    fail "unsafe generated-source entry: $name"
  canonical="$ROOT/artifact/gpgpusim_generated/$name"
  installed="$GSIM/$name"
  [[ -f "$canonical" ]] || fail "missing parser snapshot: $canonical"
  [[ -f "$installed" ]] || fail "installed parser snapshot is missing: $installed"
  if grep -Eq '^[[:space:]]*#line[[:space:]]+[0-9]+[[:space:]]+"/' "$canonical"; then
    fail "parser snapshot contains a non-reproducible absolute #line path: $name"
  fi
  cmp -s "$canonical" "$installed" || fail "installed parser snapshot differs: $name"
done

config="$GSIM/configs/tested-cfgs/SM7_QV100/gpgpusim_hbf.config"
[[ -f "$config" ]] || fail "installed HBF config is missing"
cmp -s "$ROOT/hbf/gpgpusim_hbf.config" "$config" ||
  fail "installed HBF config differs from hbf/gpgpusim_hbf.config"

grep -Eq '^-gpgpu_hbf_write_timeout_policy[[:space:]]+1([[:space:]]|$)' "$config" ||
  fail "strict incomplete-page write policy is not the installed default"
grep -Eq '^-gpgpu_deadlock_detect[[:space:]]+0([[:space:]]|$)' "$config" ||
  fail "HBF config does not disable the upstream deadlock heuristic"

printf 'artifact-check: GPGPU-Sim %s\n' "$head_commit"
printf 'artifact-check: integration patch %s\n' "$(sha256sum "$PATCH" | awk '{print $1}')"
printf 'artifact-check: %d canonical HBF sources match\n' "${#active_sources[@]}"
printf 'artifact-check: %d parser snapshots match\n' "${#generated_sources[@]}"
printf 'artifact-check: PASS\n'
