#!/usr/bin/env python3
"""04_kv_write 绘图与汇总 — 写成本、合并效率、擦除开销摊薄。"""
import csv
import os
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

HERE = os.path.dirname(os.path.abspath(__file__))


def main():
    csv_path = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "results", "results.csv")
    outdir = sys.argv[2] if len(sys.argv) > 2 else os.path.join(HERE, "results")

    with open(csv_path) as f:
        rows = list(csv.DictReader(f))
    sizes = sorted({int(r["entries"]) for r in rows})
    by_key = {(r["tier"], int(r["entries"])): r for r in rows}

    lines = [
        "| KV entries | DRAM cycles | HBF cycles | cycles/entry (HBF) | page programs | block erases | writes per program |",
        "|---:|---:|---:|---:|---:|---:|---:|",
    ]
    for s in sizes:
        rd, rh = by_key[("dram", s)], by_key[("hbf", s)]
        c_d, c_h = float(rd["cycles"]), float(rh["cycles"])
        prog = int(rh["page_programs"]) or 1
        lines.append(
            f"| {s} | {c_d:,.0f} | {c_h:,.0f} | {c_h / s:,.0f} | "
            f"{int(rh['page_programs'])} | {int(rh['block_erases'])} | "
            f"{32.0 * s / prog:.1f} |")
    table = "\n".join(lines)
    with open(os.path.join(outdir, "results.md"), "w") as f:
        f.write(table + "\n")
    print(table)

    # ---- 图: 写成本对比（log y） ----
    fig, ax = plt.subplots(figsize=(8, 5))
    xs = sizes
    ax.plot(xs, [float(by_key[("dram", s)]["cycles"]) for s in sizes],
            marker="o", label="DRAM", color="#2e86ab", linewidth=2, markersize=7)
    ax.plot(xs, [float(by_key[("hbf", s)]["cycles"]) for s in sizes],
            marker="s", label="HBF (erase+program)", color="#d1495b", linewidth=2, markersize=7)
    ax.set_xscale("log", base=2)
    ax.set_yscale("log")
    ax.set_xlabel("KV append entries (128 B each)", fontsize=12)
    ax.set_ylabel("Total cycles", fontsize=12)
    ax.set_title("KV cache append writes: DRAM vs HBF", fontsize=13)
    ax.grid(True, which="both", alpha=0.3)
    ax.legend(fontsize=11)
    fig.tight_layout()
    p = os.path.join(outdir, "write_plot.png")
    fig.savefig(p, dpi=150)
    print(f"saved {p}")


if __name__ == "__main__":
    main()
