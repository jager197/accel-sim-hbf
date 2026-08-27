#!/usr/bin/env python3
"""02_kv_cache 绘图与汇总 — 读 results.csv / window_sweep.csv，输出 results.md + 三张图。"""
import csv
import os
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

HERE = os.path.dirname(os.path.abspath(__file__))


def load_csv(path):
    with open(path) as f:
        return list(csv.DictReader(f))


def main():
    csv_path = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "results", "results.csv")
    wcsv_path = sys.argv[2] if len(sys.argv) > 2 else os.path.join(HERE, "results", "window_sweep.csv")
    outdir = sys.argv[3] if len(sys.argv) > 3 else os.path.join(HERE, "results")

    rows = load_csv(csv_path)
    by_key = {(r["case"], int(r["N"])): r for r in rows}
    ns = sorted({int(r["N"]) for r in rows})
    cases = ["unlimited", "limited", "limited_hbf"]
    labels = {
        "unlimited": "Unlimited DRAM",
        "limited": "Limited DRAM (external spill)",
        "limited_hbf": "Limited DRAM + HBF",
    }
    colors = {"unlimited": "#2e86ab", "limited": "#d1495b", "limited_hbf": "#3fa34d"}

    def cyc(c, n):
        return float(by_key[(c, n)]["cycles"])

    # ---- markdown 主表 ----
    lines = [
        "| KV entries | Overflow | Unlimited DRAM | +External spill | +HBF | spill slowdown | HBF speedup |",
        "|---:|---:|---:|---:|---:|---:|---:|",
    ]
    for n in ns:
        c_un, c_lim, c_hbf = cyc("unlimited", n), cyc("limited", n), cyc("limited_hbf", n)
        ov = float(by_key[("unlimited", n)]["overflow_pct"])
        slow = c_lim / c_un if c_un else float("nan")
        up = c_lim / c_hbf if c_hbf else float("nan")
        lines.append(f"| {n} | {ov:.0f}% | {c_un:,.0f} | {c_lim:,.0f} | {c_hbf:,.0f} | {slow:.2f}× | {up:.2f}× |")
    table = "\n".join(lines)
    with open(os.path.join(outdir, "results.md"), "w") as f:
        f.write(table + "\n")
    print(table)

    # ---- 图 1: cycles vs N（对数轴） ----
    fig, ax = plt.subplots(figsize=(8, 5.5))
    for c in cases:
        ax.plot(ns, [cyc(c, n) for n in ns], marker="o", label=labels[c],
                color=colors[c], linewidth=2, markersize=7)
    ax.set_xlabel("KV cache size (entries, 128 B each)", fontsize=12)
    ax.set_ylabel("Total simulation cycles (lower = better)", fontsize=12)
    ax.set_yscale("log")
    ax.set_title("LLM inference attention reads:\nmemory tier vs KV cache pressure", fontsize=13)
    ax.grid(True, which="both", alpha=0.3)
    ax.legend(fontsize=10)
    ax2 = ax.twiny()
    ax2.set_xlim(ax.get_xlim())
    ax2.set_xticks(ns)
    ax2.set_xticklabels([f"{float(by_key[('unlimited', n)]['overflow_pct']):.0f}%" for n in ns])
    ax2.set_xlabel("KV overflow ratio (DRAM capacity fixed)", fontsize=11)
    fig.tight_layout()
    p1 = os.path.join(outdir, "kv_plot.png")
    fig.savefig(p1, dpi=150)
    print(f"saved {p1}")

    # ---- 图 2: HBF 加速比柱状图 ----
    speedups = [cyc("limited", n) / cyc("limited_hbf", n) for n in ns]
    fig2, ax2 = plt.subplots(figsize=(8, 5))
    bars = ax2.bar([str(n) for n in ns], speedups, color=colors["limited_hbf"],
                   alpha=0.85, width=0.6)
    for b, v in zip(bars, speedups):
        ax2.text(b.get_x() + b.get_width() / 2, v * 1.03, f"{v:.2f}×",
                 ha="center", fontsize=11)
    ax2.set_xlabel("KV cache size (entries)", fontsize=12)
    ax2.set_ylabel("Speedup of HBF over external spill", fontsize=12)
    ax2.set_title("HBF vs external KV spill: speedup under pressure", fontsize=13)
    ax2.grid(True, axis="y", alpha=0.3)
    fig2.tight_layout()
    p2 = os.path.join(outdir, "speedup_plot.png")
    fig2.savefig(p2, dpi=150)
    print(f"saved {p2}")

    # ---- 图 3: window 扫描（若有数据） ----
    if os.path.exists(wcsv_path):
        wrows = load_csv(wcsv_path)
        if wrows:
            ws = sorted({int(r["window"]) for r in wrows})
            wcyc = [float({int(r["window"]): r for r in wrows}[w]["cycles"]) for w in ws]
            hits = [int({int(r["window"]): r for r in wrows}[w]["cache_hits"]) for w in ws]
            fig3, ax3 = plt.subplots(figsize=(8, 4.8))
            l1, = ax3.plot(ws, wcyc, marker="o", color=colors["limited_hbf"], linewidth=2)
            ax3.set_xlabel("Attention window (tokens)", fontsize=12)
            ax3.set_ylabel("Total cycles", color=colors["limited_hbf"], fontsize=12)
            ax3.set_title("KV attention window sweep (N=32, DRAM+HBF):\npage cache amortizes repeated reads",
                          fontsize=12)
            ax3.grid(True, alpha=0.3)
            ax3r = ax3.twinx()
            l2, = ax3r.plot(ws, hits, marker="s", color="#7b4b94", linewidth=2, linestyle="--")
            ax3r.set_ylabel("Page cache hits", color="#7b4b94", fontsize=12)
            ax3.legend([l1, l2], ["cycles", "page cache hits"], fontsize=10)
            fig3.tight_layout()
            p3 = os.path.join(outdir, "window_plot.png")
            fig3.savefig(p3, dpi=150)
            print(f"saved {p3}")


if __name__ == "__main__":
    main()
