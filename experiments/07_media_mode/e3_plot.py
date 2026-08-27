#!/usr/bin/env python3
"""E3 plot: HBF media semantics vs SSD semantics (paper SS5.3).

Focused design: only the 2048-entry case is charted (the 1024 case shows
no difference and is kept in Table 3). Zero values are drawn as thin
stubs with '0' labels, so "HBF mode: no phantom traffic" is visually
explicit instead of an empty slot.
"""
import csv
import os
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

HERE = os.path.dirname(os.path.abspath(__file__))
CSV = os.path.join(HERE, "results", "results.csv")
OUT = os.path.join(HERE, "results", "e3_mediasemantics.png")

ENTRIES = "2048"   # the discriminating case
STUB = 0.02        # stub height as a fraction of axis max (for zeros)


def main():
    rows = list(csv.DictReader(open(CSV)))
    r = {x["mode"]: x for x in rows if x["entries"] == ENTRIES}
    metrics = [
        ("gc_events", "GC events", 1),
        ("gc_page_copies", "valid-page copies", 1),
        ("gc_stall_cycles", "GC stall cycles (M)", 1e6),
        ("block_erases", "block erases", 1),
    ]
    fig, axes = plt.subplots(1, len(metrics), figsize=(9.2, 2.3))
    for ax, (col, label, unit) in zip(axes, metrics):
        vals = {"hbf": int(r["hbf"][col]), "ssd": int(r["ssd"][col])}
        vmax = max(vals.values()) or 1
        for i, (mode, color) in enumerate([("ssd", "#c0392b"), ("hbf", "#3fa34d")]):
            v = vals[mode]
            v_plot = v / unit
            h = max(v_plot, (vmax / unit) * STUB)
            ax.bar(i, h, 0.5, color=color, label=mode)
            txt = "0" if v == 0 else (f"{v_plot:,.1f}" if unit > 1 else f"{v:,}")
            ax.text(i, h * 1.05, txt, ha="center", fontsize=7)
        ax.set_xticks([0, 1], ["SSD", "HBF"], fontsize=8)
        ax.set_title(label, fontsize=9)
        ax.set_ylim(0, (vmax / unit) * 1.35)
        ax.tick_params(axis="y", labelsize=7)
    axes[0].legend(fontsize=7, loc="upper right")
    fig.suptitle(f"E3: identical overwrite stream ({ENTRIES} entries) — "
                 "SSD media semantics vs HBF (no GC)", fontsize=10)
    fig.tight_layout(rect=[0, 0, 1, 0.93])
    fig.savefig(OUT, dpi=150)
    print("saved", OUT)
    return 0


if __name__ == "__main__":
    sys.exit(main())
