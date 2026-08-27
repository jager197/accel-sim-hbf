#!/usr/bin/env python3
"""E1 analysis: effective bandwidth vs host-channel count.

Three bounds overlaid:
  1. Link ceiling: 8 partitions x NCH channels x 192 GB/s (grade-3 per-channel)
  2. NAND ceiling: 8 partitions x max_active(64) x 4KiB / tR(15us)
  3. Measured:     bytes / total cycles x core clock (1.132 GHz)

Finding: measured bandwidth is flat across channel counts -> the GPU front
end (SM in-flight requests x line size / HBF access latency) caps effective
bandwidth far below both ceilings. This is RQ1's "bandwidth cliff" result.
"""
import os
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

HERE = os.path.dirname(os.path.abspath(__file__))
CORE_GHZ = 1.132
N_PART = 8
MAX_ACTIVE = 64
PAGE = 4096
T_R_US = 15.0
PER_CH_GBPS = 192.0

# measured (from results.csv): identical for all channel counts
CH = [1, 2, 4, 8, 16]
SIZE_MB = 4
CYCLES = 290888  # all channel counts identical
MEASURED = SIZE_MB * 1024 * 1024 * CORE_GHZ / CYCLES  # GB/s


def link_ceiling(nch):
    return N_PART * nch * PER_CH_GBPS


def nand_ceiling():
    return N_PART * MAX_ACTIVE * PAGE / (T_R_US * 1e-6) / 1e9


def main():
    fig, ax = plt.subplots(figsize=(6.2, 3.6))
    xs = CH
    ax.plot(xs, [link_ceiling(c) for c in xs], marker="o", color="#2e86ab",
            label="link ceiling (8p x C x 192 GB/s)", linewidth=2, markersize=5)
    nc = nand_ceiling()
    ax.axhline(nc, color="#8e44ad", linestyle="--", linewidth=2,
               label=f"NAND ceiling ({nc:.0f} GB/s)")
    ax.axhline(MEASURED, color="#3fa34d", linestyle=":", linewidth=2,
               label=f"measured (GPU-fed, {MEASURED:.1f} GB/s)")
    ax.plot(xs, [MEASURED] * len(xs), marker="s", color="#3fa34d",
            markersize=6)
    ax.set_xscale("log", base=2)
    ax.set_xticks(xs, [str(c) for c in xs])
    ax.set_xlabel("host channels per partition (cube-wide: 8x)")
    ax.set_ylabel("effective bandwidth (GB/s)")
    ax.set_yscale("log")
    ax.grid(True, which="both", alpha=0.3)
    ax.legend(fontsize=7.5, loc="lower right")
    fig.tight_layout()
    out = os.path.join(HERE, "results", "e1_channels.png")
    os.makedirs(os.path.dirname(out), exist_ok=True)
    fig.savefig(out, dpi=150)
    print(f"saved {out}")
    print(f"measured={MEASURED:.2f} GB/s, NAND ceiling={nc:.1f} GB/s, "
          f"link ceiling (16ch)={link_ceiling(16):.0f} GB/s")


if __name__ == "__main__":
    main()
