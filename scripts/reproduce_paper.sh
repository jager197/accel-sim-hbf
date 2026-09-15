#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
exec "${PYTHON:-python3}" "$ROOT/experiments/reproduce.py" --run \
  --suite "${SUITE:-core}" --tag "${RUN_TAG:?Set a fresh RUN_TAG}" --jobs "${MAX_PAR:-1}"
