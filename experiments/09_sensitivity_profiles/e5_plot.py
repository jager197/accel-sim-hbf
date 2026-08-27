#!/usr/bin/env python3
"""E5 plot: phantom-GC stall cost across NAND timing profiles (paper S4.4).

SSD-mode bars per profile; HBF mode is zero everywhere, shown as a
dashed zero line with a label instead of empty bars.
"""
import csv
import os

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

HERE = os.path.dirname(os.path.abspath(__file__))
CSV = os.path.join(HERE, "results", "results.csv")
OUT = os.path.join(HERE, "results", "e5_profiles.png")

PROFILES = ["aggressive", "cons", "mqsim"]
LABELS = {"aggressive": "15/200/2000us", "cons": "20/500/3000us",
          "mqsim": "75/750/3800us"}


def main():
    rows = list(csv.DictReader(open(CSV)))
    ssd = []
    for pr in PROFILES:
        m = [r for r in rows if r["profile"] == pr and r["mode"] == "ssd"]
        ssd.append(int(m[0]["gc_stall_cycles"]) / 1e6)
    fig, ax = plt.subplots(figsize=(5.4, 2.6))
    ax.bar(range(3), ssd, 0.5, color="#c0392b", label="SSD semantics")
    ax.axhline(0, color="#3fa34d", linewidth=2)
    ax.text(0.02, 0.93, "HBF mode: 0 at all profiles", color="#3fa34d",
            fontsize=8, transform=ax.transAxes)
    for i, v in enumerate(ssd):
        ax.text(i, v * 1.03, f"{v:.1f}M", ha="center", fontsize=8)
    ax.set_xticks(range(3), [LABELS[p] for p in PROFILES], fontsize=8)
    ax.set_ylabel("GC stall cycles (M)", fontsize=9)
    ax.set_title("E5: phantom-GC stall cost vs NAND timing profile",
                 fontsize=9.5)
    ax.set_ylim(0, max(ssd) * 1.35)
    ax.grid(axis="y", alpha=0.3)
    ax.legend(fontsize=8)
    fig.tight_layout()
    fig.savefig(OUT, dpi=150)
    print("saved", OUT)


if __name__ == "__main__":
    main()
