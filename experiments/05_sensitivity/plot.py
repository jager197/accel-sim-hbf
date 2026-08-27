#!/usr/bin/env python3
"""05_sensitivity 绘图与汇总 — 消融柱状图 + 三组参数扫描。"""
import csv
import os
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

HERE = os.path.dirname(os.path.abspath(__file__))

VARIANT_LABELS = {
    "limited_hbf": "Full HBF",
    "hbf_nomsgr": "no MSHR",
    "hbf_nocache": "no page cache",
    "hbf_nobuffer": "no page buffer",
    "hbf_serial": "serial (max_active=1)",
}

# 每组扫描的负载说明
SERIES_WORKLOAD = {
    "cache": "kv_swa N=32 (reuse-heavy)",
    "tR": "spill tier, kv_swa N=24 (serial, no caches)",
    "active": "weight_load 4MB (streaming, no reuse)",
}


def main():
    ab_path = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "results", "ablation.csv")
    sw_path = sys.argv[2] if len(sys.argv) > 2 else os.path.join(HERE, "results", "sweeps.csv")
    outdir = sys.argv[3] if len(sys.argv) > 3 else os.path.join(HERE, "results")

    with open(ab_path) as f:
        ab = list(csv.DictReader(f))
    with open(sw_path) as f:
        sw = list(csv.DictReader(f))

    # ---- markdown ----
    lines = ["# 敏感性 / 消融", ""]
    lines.append("## A. 机制消融（kv_swa N=32 D=8 W=4）")
    lines.append("")
    lines.append("| Variant | cycles | page reads | cache hits | MSHR hits |")
    lines.append("|---:|---:|---:|---:|---:|")
    ab_by = {r["variant"]: r for r in ab}
    for v in ["limited_hbf", "hbf_nomsgr", "hbf_nocache", "hbf_nobuffer", "hbf_serial"]:
        r = ab_by[v]
        lines.append(f"| {VARIANT_LABELS[v]} | {int(r['cycles']):,} | {r['page_reads']} | "
                     f"{r['cache_hits']} | {r['mshr_hits']} |")
    lines.append("")
    lines.append("> 注: 有复用负载里页缓存吸收了大量重复读，消融的主要差异体现在"
                 "页读/缓存命中数上；tR 与并行度的效果在无复用流式负载上扫描。")
    lines.append("")
    lines.append("## B. 参数扫描")
    lines.append("")
    lines.append("| Param | value | workload | cycles |")
    lines.append("|---:|---:|---|---:|")
    for r in sw:
        lines.append(f"| {r['series']} | {r['variant']} | {SERIES_WORKLOAD.get(r['series'], '')} "
                     f"| {int(r['cycles']):,} |")
    with open(os.path.join(outdir, "results.md"), "w") as f:
        f.write("\n".join(lines) + "\n")
    print("\n".join(lines))

    # ---- 图 1: 消融柱状图 ----
    order = ["limited_hbf", "hbf_nomsgr", "hbf_nocache", "hbf_nobuffer", "hbf_serial"]
    cyc = [int(ab_by[v]["cycles"]) for v in order]
    fig, ax = plt.subplots(figsize=(8.5, 5))
    bars = ax.bar([VARIANT_LABELS[v] for v in order], cyc,
                  color=["#3fa34d", "#d1495b", "#d1495b", "#d1495b", "#d1495b"], alpha=0.85)
    for b, v in zip(bars, cyc):
        ax.text(b.get_x() + b.get_width() / 2, v * 1.03, f"{v:,}", ha="center", fontsize=10)
    ax.set_ylabel("Total cycles", fontsize=12)
    ax.set_title("Ablation: which HBF mechanism delivers the benefit\n(KV N=32, 75% overflow)",
                 fontsize=12)
    ax.grid(True, axis="y", alpha=0.3)
    fig.tight_layout()
    p = os.path.join(outdir, "ablation_plot.png")
    fig.savefig(p, dpi=150)
    print(f"saved {p}")

    # ---- 图 2: 参数扫描（三子图） ----
    fig2, axes = plt.subplots(1, 3, figsize=(14, 4.2))
    for ax2, series, xlabel in [
        (axes[0], "cache", "page cache entries"),
        (axes[1], "tR", "tR (cycles)"),
        (axes[2], "active", "max active sub-arrays"),
    ]:
        pts = sorted([(int(r["variant"]), int(r["cycles"])) for r in sw if r["series"] == series])
        xs = [p[0] for p in pts]
        ys = [p[1] for p in pts]
        ax2.plot(xs, ys, marker="o", color="#2e86ab", linewidth=2)
        ax2.set_xlabel(xlabel, fontsize=11)
        ax2.set_ylabel("cycles", fontsize=11)
        ax2.set_title(f"{series} sweep\n({SERIES_WORKLOAD[series]})", fontsize=10)
        ax2.grid(True, alpha=0.3)
    fig2.tight_layout()
    p2 = os.path.join(outdir, "sweeps_plot.png")
    fig2.savefig(p2, dpi=150)
    print(f"saved {p2}")


if __name__ == "__main__":
    main()
