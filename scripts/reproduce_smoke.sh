#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RUN_TAG="${RUN_TAG:-}" bash "$ROOT/experiments/10_validation/run.sh"
