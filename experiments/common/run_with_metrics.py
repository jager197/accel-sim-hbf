#!/usr/bin/env python3
"""Run one command and record portable host-side resource measurements."""

from __future__ import annotations

import os
import resource
import subprocess
import sys
import time
from pathlib import Path


def main() -> int:
    if len(sys.argv) < 3:
        print("usage: run_with_metrics.py OUTPUT COMMAND [ARGS...]", file=sys.stderr)
        return 2

    output = Path(sys.argv[1])
    before = resource.getrusage(resource.RUSAGE_CHILDREN)
    start = time.perf_counter()
    completed = subprocess.run(sys.argv[2:], check=False)
    elapsed = time.perf_counter() - start
    after = resource.getrusage(resource.RUSAGE_CHILDREN)

    # Linux reports ru_maxrss in KiB. HBF-Sim's artifact container is Linux;
    # record the platform explicitly so archived measurements stay interpretable.
    lines = [
        "timer=python-resource",
        f"platform={sys.platform}",
        f"wall_seconds={elapsed:.6f}",
        f"user_seconds={after.ru_utime - before.ru_utime:.6f}",
        f"system_seconds={after.ru_stime - before.ru_stime:.6f}",
        f"max_rss_kib={after.ru_maxrss}",
        f"exit_code={completed.returncode}",
    ]
    output.write_text("\n".join(lines) + "\n", encoding="ascii")
    return completed.returncode


if __name__ == "__main__":
    raise SystemExit(main())
