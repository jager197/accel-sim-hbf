#!/usr/bin/env python3
"""ideal_bw.py — HBF 顺序读带宽的"理想情况"解析模型（不跑仿真）

模型:
  层级带宽上限(GB/s) = 分区数 × max_active × 页大小 / tR × 核心频率
                     = 8 × max_active × 4KB / tR × 1.132GHz

对照线:
  - DRAM 实测带宽带（83–182 GB/s，来自 03 最终运行）
  - 前端喂流上限 ≈ 14.4 GB/s（信用放大后的实测值；GPGPU-Sim SM 前端
    在飞跟踪的结构性上限，属于模拟器核心，待后续修复）

产物:
  results/ideal_plot.png        三合一: 带宽-并行度 / 带宽-tR / 权重加载时间
  results/ideal_results.md      关键交叉点数值
"""
import os
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib import font_manager
import numpy as np

# 注册 CJK 字体（系统只有 DroidSansFallback），否则图中中文显示为方块。
# 字体列表: DejaVu Sans 负责拉丁字符，Droid Sans Fallback 回退中文。
for _f in ("/usr/share/fonts/truetype/droid/DroidSansFallbackFull.ttf",):
    if os.path.exists(_f):
        font_manager.fontManager.addfont(_f)
        plt.rcParams["font.sans-serif"] = ["DejaVu Sans", "Droid Sans Fallback"]
        plt.rcParams["axes.unicode_minus"] = False
        break

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "results")
os.makedirs(OUT, exist_ok=True)

CORE_GHZ = 1.132      # gpgpu_clock_domains 1132 MHz
PAGE = 4096           # NAND 页大小
N_PART = 8            # SMALL_GPU: 8 个 memory partition
DRAM_LO, DRAM_HI = 83.2, 181.8        # DRAM 实测带宽带
FEED_LIMIT = 14.4     # 前端喂流实测上限（信用放大后，16MB）
T_R_DEFAULT = 15000   # 默认 tR
MAX_ACTIVE_DEFAULT = 64


def tier_bw(max_active, tR):
    """理想层级带宽上限 (GB/s)"""
    return N_PART * max_active * PAGE / tR * CORE_GHZ


def main():
    # ═══ 图 1: 带宽 vs max_active（并行度） ═══
    fig, axes = plt.subplots(1, 3, figsize=(16.5, 4.8))

    ax = axes[0]
    max_act = np.logspace(1, 4, 100)   # 8 .. 16384
    for tR, c in [(5000, "#2e86ab"), (15000, "#3fa34d"), (30000, "#d1495b")]:
        ax.plot(max_act, tier_bw(max_act, tR), label=f"tR={tR:,} cycles", color=c,
                linewidth=2)
    ax.axhspan(DRAM_LO, DRAM_HI, color="#f2b705", alpha=0.25, label="DRAM 实测带")
    ax.axhline(FEED_LIMIT, color="gray", linestyle=":", linewidth=1.5,
               label=f"前端喂流上限 ≈ {FEED_LIMIT} GB/s")
    # 当前配置点
    cur = tier_bw(MAX_ACTIVE_DEFAULT, T_R_DEFAULT)
    ax.plot(MAX_ACTIVE_DEFAULT, cur, marker="*", markersize=14, color="#3fa34d",
            zorder=5, label=f"当前配置 (max_active=64, tR=15K) → {cur:,.0f} GB/s")
    # 与 DRAM 打平的交叉点
    for tR in (5000, 15000, 30000):
        cross = DRAM_HI * tR / (N_PART * PAGE * CORE_GHZ)
        ax.axvline(cross, color="gray", linestyle="--", alpha=0.4, linewidth=0.8)
    ax.set_xscale("log")
    ax.set_xlabel("max_active（同时活跃子阵列数）", fontsize=10)
    ax.set_ylabel("顺序读带宽 (GB/s)", fontsize=10)
    ax.set_title("理想带宽 vs 并行度\n(与 HBM 打平只需 max_active ≈ 60–120)", fontsize=10)
    ax.grid(True, which="both", alpha=0.3)
    ax.legend(fontsize=7, loc="upper left")

    # ═══ 图 2: 带宽 vs tR ═══
    ax = axes[1]
    tRs = np.linspace(3000, 40000, 100)
    for ma, c in [(64, "#3fa34d"), (256, "#2e86ab"), (1024, "#7b4b94")]:
        ax.plot(tRs, tier_bw(ma, tRs), label=f"max_active={ma}", color=c, linewidth=2)
    ax.axhspan(DRAM_LO, DRAM_HI, color="#f2b705", alpha=0.25, label="DRAM 实测带")
    ax.axhline(FEED_LIMIT, color="gray", linestyle=":", linewidth=1.5,
               label=f"前端喂流上限 ≈ {FEED_LIMIT} GB/s")
    ax.plot(T_R_DEFAULT, tier_bw(MAX_ACTIVE_DEFAULT, T_R_DEFAULT), marker="*",
            markersize=14, color="#3fa34d", zorder=5)
    ax.set_xlabel("tR（NAND 页读延迟, cycles）", fontsize=10)
    ax.set_ylabel("顺序读带宽 (GB/s)", fontsize=10)
    ax.set_title("理想带宽 vs tR\n(tR 是带宽的线性除数)", fontsize=10)
    ax.grid(True, alpha=0.3)
    ax.legend(fontsize=7, loc="upper right")

    # ═══ 图 3: 权重加载时间 ═══
    ax = axes[2]
    sizes = np.array([7, 13, 70, 175, 540])   # LLaMA-7B/13B/70B/175B/GPT-3
    names = ["7B", "13B", "70B", "175B", "540B"]
    drm = sizes / 150.0
    ideal = sizes / tier_bw(MAX_ACTIVE_DEFAULT, T_R_DEFAULT)
    fed = sizes / FEED_LIMIT
    ax.plot(sizes, drm, marker="o", label="DRAM @150 GB/s", color="#f2b705", linewidth=2)
    ax.plot(sizes, ideal, marker="s", label=f"HBF 理想 @{tier_bw(MAX_ACTIVE_DEFAULT, T_R_DEFAULT):.0f} GB/s",
            color="#3fa34d", linewidth=2)
    ax.plot(sizes, fed, marker="^", label=f"HBF 前端受限 @{FEED_LIMIT} GB/s",
            color="gray", linewidth=2, linestyle="--")
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_xticks(sizes, names)
    ax.set_xlabel("模型权重规模", fontsize=10)
    ax.set_ylabel("顺序加载时间 (s)", fontsize=10)
    ax.set_title("权重加载时间（prefill 场景）\n理想 HBF 与 DRAM 相当；前端受限则差 ~10×", fontsize=10)
    ax.grid(True, which="both", alpha=0.3)
    ax.legend(fontsize=7)

    fig.tight_layout()
    p = os.path.join(OUT, "ideal_plot.png")
    fig.savefig(p, dpi=150)
    print(f"saved {p}")

    # ═══ 关键数值 → markdown ═══
    cross64 = DRAM_HI * T_R_DEFAULT / (N_PART * PAGE * CORE_GHZ)
    lines = [
        "# 03 理想性能模型（解析，不跑仿真）",
        "",
        f"- 层级带宽公式: 8 分区 × max_active × 4KB / tR × 1.132GHz",
        f"- 当前配置 (max_active=64, tR=15K): 理想 **{tier_bw(64, 15000):,.0f} GB/s**，"
        f"前端受限实测 **{FEED_LIMIT} GB/s**，DRAM 实测 **{DRAM_LO:,.0f}–{DRAM_HI:,.0f} GB/s**",
        "",
        "| 场景 | HBF 理想带宽 | 与 HBM 打平条件 |",
        "|---|---|---|",
        f"| tR=15K（默认） | 8×max_active×4KB/15K×1.132 | max_active ≈ {cross64:.0f} |",
        f"| tR=5K（快 NAND） | 8×max_active×4KB/5K×1.132 | max_active ≈ {DRAM_HI*5000/(N_PART*PAGE*CORE_GHZ):.0f} |",
        f"| max_active=256 | 8×256×4KB/tR×1.132 | tR ≤ {8*256*PAGE*CORE_GHZ/DRAM_HI:,.0f} cycles |",
        "",
        "> 结论: HBF 顺序读带宽在模型层面完全够得着 HBM —— 当前配置的理论上限"
        " 158 GB/s 与 DRAM 实测同量级；差距全部来自 GPGPU-Sim 前端喂流"
        "（在飞请求 ~3K，实测封顶 ~14 GB/s），待后续修复前端或加合成直注测试。",
        "",
        f"图: [ideal_plot.png](ideal_plot.png)",
    ]
    with open(os.path.join(OUT, "ideal_results.md"), "w") as f:
        f.write("\n".join(lines) + "\n")
    print("\n".join(lines))


if __name__ == "__main__":
    main()
