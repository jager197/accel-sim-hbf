#!/usr/bin/env python3
"""Plot the channel-sweep validation bundle from its fresh CSV."""

import argparse
import csv
import hashlib
import json
from pathlib import Path

import matplotlib.pyplot as plt


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def load_rows(path: Path):
    with path.open(newline="") as stream:
        rows = list(csv.DictReader(stream))
    if not rows:
        raise ValueError(f"empty validation CSV: {path}")
    required = {"channels", "cycles", "hbf_requests", "page_reads", "mshr_hits", "trace_requests", "trace_valid"}
    missing = required.difference(rows[0])
    if missing:
        raise ValueError(f"missing columns: {sorted(missing)}")
    for row in rows:
        for key in required:
            if key != "trace_valid":
                row[key] = float(row[key])
        row["trace_valid"] = int(row["trace_valid"])
    return rows


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("csv", type=Path)
    parser.add_argument("--out-dir", type=Path)
    args = parser.parse_args()
    out_dir = args.out_dir or args.csv.parent
    out_dir.mkdir(parents=True, exist_ok=True)
    rows = load_rows(args.csv)
    expected_channels = [1, 2, 4, 8, 16]
    observed_channels = [int(row["channels"]) for row in rows]
    if observed_channels != expected_channels:
        raise ValueError(
            f"expected ordered channel sweep {expected_channels}, got {observed_channels}"
        )
    if any(row["trace_valid"] != 1 for row in rows):
        raise ValueError("refusing to plot an invalid trace row")
    for row in rows:
        for key in ("cycles", "hbf_requests", "page_reads", "trace_requests"):
            if row[key] <= 0:
                raise ValueError(f"{key} must be positive for channels={row['channels']}")

    x = [row["channels"] for row in rows]
    fig, axes = plt.subplots(1, 2, figsize=(6.8, 2.7), constrained_layout=True)
    axes[0].plot(x, [row["cycles"] for row in rows], marker="o", label="GPU cycles")
    axes[0].set_xlabel("Logical-cube channels")
    axes[0].set_ylabel("Simulation cycles")
    axes[0].set_xticks(x)
    axes[0].grid(axis="y", alpha=0.3)
    axes[1].plot(x, [row["page_reads"] for row in rows], marker="o", label="Page reads")
    axes[1].plot(x, [row["mshr_hits"] for row in rows], marker="s", label="MSHR hits")
    axes[1].set_xlabel("Logical-cube channels")
    axes[1].set_ylabel("Count")
    axes[1].set_xticks(x)
    axes[1].grid(axis="y", alpha=0.3)
    axes[1].legend(frameon=False, fontsize=8)
    fig.savefig(out_dir / "channel_sweep.pdf", metadata={"Creator": "HBF-Sim validation"})
    fig.savefig(out_dir / "channel_sweep.png", dpi=220)
    plt.close(fig)

    manifest = {
        "input_csv": str(args.csv.resolve()),
        "input_sha256": sha256(args.csv),
        "rows": len(rows),
        "outputs": ["channel_sweep.pdf", "channel_sweep.png"],
    }
    (out_dir / "channel_sweep.meta.json").write_text(
        json.dumps(manifest, indent=2) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
