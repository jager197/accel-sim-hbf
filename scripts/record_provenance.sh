#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 ]]; then
  echo "usage: $0 OUTPUT_DIR [INPUT ...]" >&2
  exit 2
fi

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT="$1"
shift
mkdir -p "$OUT"
PYTHON="${PYTHON:-python3}"

git_value() {
  local repo="$1"; shift
  git -C "$repo" "$@" 2>/dev/null || printf 'unavailable\n'
}

{
  printf 'generated_utc=%s\n' "$(date -u '+%Y-%m-%dT%H:%M:%SZ')"
  printf 'hostname=%s\n' "$(hostname)"
  printf 'kernel=%s\n' "$(uname -srmo)"
  printf 'repo_head=%s\n' "$(git_value "$ROOT" rev-parse HEAD)"
  printf 'repo_dirty_files=%s\n' "$(git -C "$ROOT" status --porcelain 2>/dev/null | wc -l)"
  printf 'repo_status_sha256=%s\n' "$(git -C "$ROOT" status --porcelain=v1 -z 2>/dev/null | sha256sum | awk '{print $1}')"
  printf 'repo_diff_sha256=%s\n' "$(git -C "$ROOT" diff --binary HEAD -- 2>/dev/null | sha256sum | awk '{print $1}')"
  if [[ -d "$ROOT/gpu-simulator/gpgpu-sim/.git" ]]; then
    printf 'gpgpusim_head=%s\n' "$(git_value "$ROOT/gpu-simulator/gpgpu-sim" rev-parse HEAD)"
    printf 'gpgpusim_dirty_files=%s\n' "$(git -C "$ROOT/gpu-simulator/gpgpu-sim" status --porcelain 2>/dev/null | wc -l)"
    printf 'gpgpusim_status_sha256=%s\n' "$(git -C "$ROOT/gpu-simulator/gpgpu-sim" status --porcelain=v1 -z 2>/dev/null | sha256sum | awk '{print $1}')"
    printf 'gpgpusim_diff_sha256=%s\n' "$(git -C "$ROOT/gpu-simulator/gpgpu-sim" diff --binary HEAD -- 2>/dev/null | sha256sum | awk '{print $1}')"
  fi
  printf 'cuda_install_path=%s\n' "${CUDA_INSTALL_PATH:-unset}"
  if command -v nvcc >/dev/null 2>&1; then
    printf 'nvcc=%s\n' "$(nvcc --version | awk -F'release ' '/release/{print $2; exit}' | tr -d ',')"
  else
    printf 'nvcc=missing\n'
  fi
  printf 'python=%s\n' "$($PYTHON --version 2>&1)"
  printf 'compiler=%s\n' "$(c++ --version 2>/dev/null | head -1 || echo missing)"
  printf 'command='; printf '%q ' "${PROVENANCE_COMMAND:-$0}"; printf '\n'
} > "$OUT/provenance.env"

hash_untracked() {
  local repo="$1" destination="$2"
  : > "$destination"
  while IFS= read -r -d '' path; do
    if [[ -f "$repo/$path" ]]; then
      (cd "$repo" && sha256sum -- "$path") >> "$destination"
    fi
  done < <(git -C "$repo" ls-files --others --exclude-standard -z 2>/dev/null)
}

hash_untracked "$ROOT" "$OUT/repo_untracked.sha256"
if [[ -d "$ROOT/gpu-simulator/gpgpu-sim/.git" ]]; then
  hash_untracked "$ROOT/gpu-simulator/gpgpu-sim" "$OUT/gpgpusim_untracked.sha256"
fi

: > "$OUT/inputs.sha256"
missing=0
for input in "$@"; do
  if [[ -f "$input" ]]; then
    sha256sum "$input" >> "$OUT/inputs.sha256"
  else
    printf 'MISSING  %s\n' "$input" >> "$OUT/inputs.sha256"
    missing=$((missing + 1))
  fi
done

(( missing == 0 )) || {
  echo "provenance: $missing declared input(s) are missing" >&2
  exit 1
}

echo "provenance: $OUT/provenance.env"
