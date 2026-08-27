#!/usr/bin/env python3
"""bw_chart.py — 03 顺序读带宽图（实测数据风格，与 03 原图同款）

数据来源: 03 最终运行（页级交错 + 信用放大配置）的实测值，见
experiments/03_weight_load/runs/*/run.log。

误差棒: ±1%。依据: 16MB HBF 三次复跑观测到的 run-to-run 波动 < 0.2%
（1,319,217 / 1,314,888 / 1,314,747 cycles），误差棒取 ±1% 保守值。

产物: results/bw_chart.png
"""
import os

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib import font_manager

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "results")
os.makedirs(OUT, exist_ok=True)

# CJK 字体回退（DejaVu Sans 负责拉丁字符，Droid Sans Fallback 负责中文）
_FONT = "/usr/share/fonts/truetype/droid/DroidSansFallbackFull.ttf"
if os.path.exists(_FONT):
    font_manager.fontManager.addfont(_FONT)
    plt.rcParams["font.sans-serif"] = ["DejaVu Sans", "Droid Sans Fallback"]
    plt.rcParams["axes.unicode_minus"] = False

# 03 最终运行实测值（GB/s）
SIZES = [1, 4, 8, 16]
DRAM = {1: 83.2, 4: 145.6, 8: 169.4, 16: 181.8}
HBF = {1: 12.7, 4: 14.0, 8: 14.3, 16: 14.4}
ERR = 0.01  # ±1% 保守误差棒


def main():
    fig, ax = plt.subplots(figsize=(8, 5))

    for data, label, color, marker in [
        (DRAM, "DRAM (HBM)", "#2e86ab", "o"),
        (HBF, "HBF", "#3fa34d", "s"),
    ]:
        ys = [data[s] for s in SIZES]
        yerr = [v * ERR for v in ys]
        ax.errorbar(SIZES, ys, yerr=yerr, label=label, color=color,
                    marker=marker, markersize=7, linewidth=2,
                    capsize=4, capthick=1.2)

    ax.set_xlabel("Sequential load size (MB)", fontsize=12)
    ax.set_ylabel("Effective bandwidth (GB/s)", fontsize=12)
    ax.set_title("LLM weight / prefill loading:\nDRAM vs HBF effective bandwidth",
                 fontsize=13)
    ax.set_xticks(SIZES)
    ax.grid(True, alpha=0.3)
    ax.legend(fontsize=11)
    ax.text(0.02, 0.03,
            "error bars: ±1% (3 reruns observed <0.2% spread)",
            transform=ax.transAxes, fontsize=8, color="gray")

    fig.tight_layout()
    p = os.path.join(OUT, "bw_chart.png")
    fig.savefig(p, dpi=150)
    print(f"saved {p}")

    # 数值表（论文/汇报用）
    lines = [
        "| Size (MB) | DRAM GB/s | HBF GB/s |",
        "|---:|---:|---:|",
    ]
    for s in SIZES:
        lines.append(f"| {s} | {DRAM[s]:.1f} ±{DRAM[s]*ERR:.1f} | "
                     f"{HBF[s]:.1f} ±{HBF[s]*ERR:.2f} |")
    table = "\n".join(lines)
    with open(os.path.join(OUT, "bw_chart.md"), "w") as f:
        f.write(table + "\n")
    print(table)


if __name__ == "__main__":
    main()
