#!/usr/bin/env python3
"""arch_fig.py — accel-sim-hbf 架构图（论文风格 v3: 严格网格对齐/统一尺寸/浅灰标示新增组件）

产物: docs/architecture.png (+ .pdf)
"""
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch, Polygon, Rectangle
from matplotlib.backends.backend_pdf import PdfPages
import os

HERE = os.path.dirname(os.path.abspath(__file__))
OUT_PNG = os.path.join(HERE, "architecture.png")
OUT_PDF = os.path.join(HERE, "architecture.pdf")

INK = "#1a1a1a"          # 主线条
SUB = "#3c3c3c"          # 次级文字
SHADE = "#ececec"        # 新增组件浅灰底
SHADE_EC = "#555555"

S = {"tab": 8.5, "box": 8, "sub": 6.8, "cap": 8}


def box(ax, x, y, w, h, fc="white", ec=INK, lw=1.0, ls="-"):
    ax.add_patch(FancyBboxPatch((x, y), w, h,
                                boxstyle="round,pad=0.1,rounding_size=0.55",
                                fc=fc, ec=ec, lw=lw, linestyle=ls, zorder=2))


def txt(ax, x, y, s, size=8, weight="normal", color=INK, ha="center",
        va="center"):
    ax.text(x, y, s, fontsize=size, fontweight=weight, color=color,
            ha=ha, va=va, zorder=3, linespacing=1.25)


def arrow(ax, x1, y1, x2, y2, lw=1.0):
    ax.add_patch(FancyArrowPatch((x1, y1), (x2, y2), arrowstyle="-|>",
                                 mutation_scale=8, color=INK, lw=lw, zorder=4))


def elbow(ax, pts, lw=1.0):
    """pts: [(x1,y1), (x2,y2), ...] 折线；最后一段带箭头"""
    for (x1, y1), (x2, y2) in zip(pts[:-2], pts[1:-1]):
        ax.add_patch(FancyArrowPatch((x1, y1), (x2, y2), arrowstyle="-",
                                     color=INK, lw=lw, zorder=4))
    (x1, y1), (x2, y2) = pts[-2], pts[-1]
    ax.add_patch(FancyArrowPatch((x1, y1), (x2, y2), arrowstyle="-|>",
                                 mutation_scale=8, color=INK, lw=lw, zorder=4))


def main():
    fig, ax = plt.subplots(figsize=(7.6, 5.9))
    ax.set_xlim(0, 100)
    ax.set_ylim(0, 74)
    ax.axis("off")
    fig.subplots_adjust(left=0.01, right=0.99, top=0.99, bottom=0.01)

    # ═══ 1. 负载（顶行） ═══
    box(ax, 22, 66, 56, 6.4)
    txt(ax, 50, 70.4, "LLM Inference Workloads", S["box"], "bold")
    txt(ax, 50, 68.0, "kv_swa · weight_load · kv_write", S["sub"], color=SUB)

    # ═══ 2. GPU 时序模型 ═══
    box(ax, 8, 51, 84, 12.4, fc="#fafafa", lw=1.1)
    txt(ax, 10.6, 61.9, "GPGPU-Sim 4.2 Timing Model (PTX mode)", S["tab"],
        "bold", ha="left")
    box(ax, 16, 53, 28, 6.6)
    txt(ax, 30, 57.4, "SM Cores", S["box"], "bold")
    txt(ax, 30, 54.9, "issue · scoreboard · LSU", S["sub"], color=SUB)
    box(ax, 56, 53, 28, 6.6)
    txt(ax, 70, 57.4, "L1D + Interconnect", S["box"], "bold")
    txt(ax, 70, 54.9, "crossbar network", S["sub"], color=SUB)
    arrow(ax, 44, 56.3, 56, 56.3)
    txt(ax, 50, 57.7, "64B requests", S["sub"], color=SUB)

    arrow(ax, 50, 65.8, 50, 63.6)      # 负载 → GPU
    arrow(ax, 50, 50.8, 50, 48.9)      # GPU → 分区

    # ═══ 3. 内存分区（含路由） ═══
    box(ax, 8, 38, 84, 10.6, fc="#fafafa", lw=1.1)
    txt(ax, 10.6, 47.2, "Memory Partition ×8", S["tab"], "bold", ha="left")
    box(ax, 16, 40.2, 22, 5.6)
    txt(ax, 27, 44.0, "L2 Cache", S["box"], "bold")
    txt(ax, 27, 41.9, "write-back", S["sub"], color=SUB)
    ax.add_patch(Polygon([(55, 45.9), (60.4, 43.0), (55, 40.1), (49.6, 43.0)],
                         closed=True, fc="white", ec=INK, lw=1.0, zorder=2))
    txt(ax, 55, 43.0, "is_hbf_\naddr()?", S["sub"], color=SUB)
    arrow(ax, 38, 43.0, 49.6, 43.0)
    txt(ax, 43.8, 44.6, "L2 miss", S["sub"], color=SUB)

    # 分流（从菱形底角出，跨容器边界 → 水平 → 下探）
    elbow(ax, [(52.4, 40.2), (52.4, 36.6), (25, 36.6), (25, 34.6)])
    txt(ax, 38.7, 37.8, "DRAM region", S["sub"], color=SUB)
    elbow(ax, [(57.6, 40.2), (57.6, 36.6), (75, 36.6), (75, 34.6)])
    txt(ax, 66.3, 37.8, "HBF region", S["sub"], color=SUB)

    # ═══ 4a. DRAM 列（左） ═══
    box(ax, 12, 27.4, 26, 6.8)
    txt(ax, 25, 31.6, "DRAM Controller", S["box"], "bold")
    txt(ax, 25, 29.1, "FR-FCFS (HBM stand-in)", S["sub"], color=SUB)
    box(ax, 12, 17.8, 26, 6.4)
    txt(ax, 25, 22.0, "DRAM Banks", S["box"], "bold")
    txt(ax, 25, 19.5, "nbk=16 · ~200 GB/s", S["sub"], color=SUB)
    arrow(ax, 25, 27.2, 25, 24.4)

    # ═══ 4b. HBF 列（右，新增组件浅灰） ═══
    box(ax, 52, 25.2, 36, 9.0, fc=SHADE, ec=SHADE_EC, lw=1.1)
    txt(ax, 70, 33.0, "HBF Controller", S["box"], "bold")
    txt(ax, 70, 30.9, "host channels ×C (UCIe/AXI)", S["sub"], color=SHADE_EC)
    # 4 个等宽子框
    subs = [("MSHR", "64B→4KB"), ("Page\nCache", "LRU"),
            ("FTL", "no-GC/zone"), ("Sched.", "chan-affine")]
    bx, bw, gap = 53.4, 7.6, 0.7
    for i, (t, d) in enumerate(subs):
        x0 = bx + i * (bw + gap)
        box(ax, x0, 26.4, bw, 3.6, fc="white", ec=SHADE_EC, lw=0.9)
        txt(ax, x0 + bw / 2, 28.9, t, 7, "bold", color=INK)
        txt(ax, x0 + bw / 2, 27.1, d, 5.6, color=SUB)
    for i in range(3):
        x0 = bx + (i + 1) * (bw + gap) - gap
        arrow(ax, x0, 28.2, x0 + gap, 28.2, lw=0.8)

    box(ax, 52, 13.6, 36, 7.6, fc=SHADE, ec=SHADE_EC, lw=1.1)
    txt(ax, 70, 19.4, "NAND Sub-array Array (×16,384)", S["box"], "bold")
    # 小方块阵列图标
    for i in range(6):
        for j in range(2):
            ax.add_patch(Rectangle((57.0 + i * 1.9, 15.2 + j * 1.6), 1.4, 1.1,
                                   fc="white", ec=SHADE_EC, lw=0.7, zorder=2))
    txt(ax, 70.5, 16.4, "……", S["box"], color=SHADE_EC)
    txt(ax, 70, 15.1, "tR=15K · tPROG=200K · tBERS=2M", S["sub"], color=SUB)

    arrow(ax, 70, 25.0, 70, 21.4)
    txt(ax, 73.6, 23.2, "4KB page read", S["sub"], color=SUB, ha="left")

    # ═══ 图注 ═══
    txt(ax, 50, 5.2, "Figure 1. Overview of the HBF-integrated GPU simulator "
        "(accel-sim-hbf). Shaded components are added in this work.",
        S["cap"], color=SUB)
    txt(ax, 50, 8.2, "Address range 256GB–768GB is routed to HBF; the rest "
        "follows the DRAM path.", S["sub"], color=SUB)

    fig.savefig(OUT_PNG, dpi=200)
    with PdfPages(OUT_PDF) as pdf:
        pdf.savefig(fig)
    print(f"saved {OUT_PNG}")
    print(f"saved {OUT_PDF}")


if __name__ == "__main__":
    main()
