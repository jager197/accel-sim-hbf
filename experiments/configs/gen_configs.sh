#!/bin/bash
# ============================================================================
# experiments/configs/gen_configs.sh — 生成全部实验配置
#
# 从 HBF 基础配置（SM7_QV100/gpgpusim_hbf.config）派生。所有配置都采用
# "小 GPU" 变体（8 SM / 8 memory partition）以加速仿真——负载均为少量
# warp，SM 数不影响结果，与旧实验口径一致。
#
# 内存层级场景（v0.4 起）:
#   unlimited.config    纯 DRAM（HBF 关闭）—— 容量无限基准
#   limited.config      历史外溢基线（串行/无合并/无缓存/tR=20K）——
#                       仅作向后兼容，论文不再使用（被 limited_nvme 取代）
#   limited_nvme.config 公平的 NVMe 式外溢基线：单通道链路限速 3.5 GB/s
#                       （PCIe 4.0 x4 级）、4KiB 事务（无 MSHR 合并）、
#                       100us 级访问、8 路介质并行（NVMe die 并行度级）
#   limited_hbf.config  DRAM 容量受限 → 溢出到 HBF（v0.4 完整模型：
#                       4 Host Channel + MSHR 合并 + 页缓存 + 页缓冲 +
#                       无 GC zone 介质管理 + 前端信用放大）
#
# 语义对照（论文 E3: "SSD 模型误判 HBF"）:
#   hbf_ssd_mode.config limited_hbf 但 media_mode=1（GREEDY GC + 搬移）
#
# 通道研究（论文 E1/E2）:
#   hbf_ch{1,2,4,8,16}.config  通道数扫描（E1 带宽 vs 并行度）
#   hbf_ch_partition.config    通道分区映射（E2: 权重/KV 分通道，§13.3.3）
#
# 消融变体（05_sensitivity）:
#   hbf_nomsgr.config   limited_hbf 去掉 MSHR 合并
#   hbf_nocache.config  limited_hbf 去掉共享页缓存
#   hbf_nobuffer.config limited_hbf 去掉子阵列页缓冲
#   hbf_serial.config   limited_hbf 子阵列并行度降为 1
# ============================================================================
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/../common/env.sh"

BASE="$GPGPUSIM_ROOT/configs/tested-cfgs/SM7_QV100/gpgpusim_hbf.config"
OUT_DIR="$SCRIPT_DIR"
mkdir -p "$OUT_DIR"

if [ ! -f "$BASE" ]; then
    echo "Error: base config not found: $BASE" >&2
    echo "Run: source gpu-simulator/setup_environment.sh && bash setup_hbf.sh" >&2
    exit 1
fi

# 死锁检测：单 warp 长时间等待慢速存储是合法行为，必须关闭
DEADLOCK_OFF='-gpgpu_deadlock_detect 0'
# 小 GPU：8 SM / 8 分区（旧实验同款，加速 ~10x）
SMALL_GPU='s/^-gpgpu_n_clusters .*/ -gpgpu_n_clusters 8/; s/^-gpgpu_n_mem .*/ -gpgpu_n_mem 8/'
# L1D 旁路：真实推理中 KV/权重工作集（GB 级）远大于 L1（~128KB），
# 所有访问必然 miss L1。实验统一绕过 L1D，让请求直达 L2/存储层，
# 保证测量的是"内存层级"的时序而非 L1 命中。
SKIP_L1D='s/^-gpgpu_gmem_skip_L1D .*/ -gpgpu_gmem_skip_L1D 1/'
# 页级地址交错：把分区选择位从 bit 8（256B 交错）移到 bit 12（4KB 交错）。
# 旧映射下同一 NAND 页的 16 个 256B 条带洒进 8 个分区 → 每个分区各自
# 触发一次完整页读 → 同一页被读 8 遍（实测 16MB 页读 32768 vs 理论 4096）。
# 页级交错后一个 4KB 页整体只归一个分区，页读次数 = 数据量/4KB。
PAGE_MAP='s/dramid@8;/dramid@12;/'
# v0.4 前端信用放大（T6）：GPGPU-Sim 的 L2/仲裁/返回队列默认尺寸
# （64/64/192）把 HBF 有效带宽压到 ~5.6 GB/s；放大到 4096 后
# 实测提升到 14.4 GB/s，且瓶颈转移到 SM 在飞请求上限（论文 §6.1
# "GPU 前端喂流" 归因分析的配置基础）。
FRONTEND_BOOST='s/^-gpgpu_dram_partition_queues .*/ -gpgpu_dram_partition_queues 4096:4096:4096:4096/; s/^-gpgpu_frfcfs_dram_sched_queue_size .*/ -gpgpu_frfcfs_dram_sched_queue_size 4096/; s/^-gpgpu_dram_return_queue_size .*/ -gpgpu_dram_return_queue_size 4096/; s/^-gpgpu_hbf_max_outstanding .*/ -gpgpu_hbf_max_outstanding 4096/'

gen() {  # gen <name> <sed-args...>   （-e 'expr' 形式）
    local name="$1"; shift
    sed "$@" -e "$SMALL_GPU" -e "$SKIP_L1D" -e "$PAGE_MAP" "$BASE" > "$OUT_DIR/$name.config"
    echo "$DEADLOCK_OFF" >> "$OUT_DIR/$name.config"
    echo "  $name.config"
}

# 旧映射对照（256B 交错，保留 dramid@8）—— 用于 03 的"修复前后"对比
gen_oldmap() {  # gen_oldmap <name> <sed-args...>
    local name="$1"; shift
    sed "$@" -e "$SMALL_GPU" -e "$SKIP_L1D" "$BASE" > "$OUT_DIR/$name.config"
    echo "$DEADLOCK_OFF" >> "$OUT_DIR/$name.config"
    echo "  $name.config (256B 旧交错)"
}

echo "生成配置（$OUT_DIR）:"

# 1. unlimited — 纯 DRAM
gen unlimited \
    -e 's/^-gpgpu_hbf_enabled .*/ -gpgpu_hbf_enabled 0/'

# 2. limited — 历史外溢基线（向后兼容，论文不再使用）
gen limited \
    -e 's/^-gpgpu_hbf_enabled .*/ -gpgpu_hbf_enabled 1/' \
    -e 's/^-gpgpu_hbf_tR .*/ -gpgpu_hbf_tR 20000/' \
    -e 's/^-gpgpu_hbf_mshr_enabled .*/ -gpgpu_hbf_mshr_enabled 0/' \
    -e 's/^-gpgpu_hbf_cache_entries .*/ -gpgpu_hbf_cache_entries 0/' \
    -e 's/^-gpgpu_hbf_buffer_enabled .*/ -gpgpu_hbf_buffer_enabled 0/' \
    -e 's/^-gpgpu_hbf_max_active .*/ -gpgpu_hbf_max_active 1/'

# 3. limited_nvme — 公平的 NVMe 式外溢基线（v0.4，论文主基线）
#    PCIe 4.0 x4 级链路：单通道 3.5 GB/s；4KiB 事务（无 MSHR 合并）；
#    100us 级访问（tR=85000 ticks @ 850MHz）；8 路介质并行（NVMe 级）。
gen limited_nvme \
    -e 's/^-gpgpu_hbf_enabled .*/ -gpgpu_hbf_enabled 1/' \
    -e 's/^-gpgpu_hbf_tR .*/ -gpgpu_hbf_tR 85000/' \
    -e 's/^-gpgpu_hbf_mshr_enabled .*/ -gpgpu_hbf_mshr_enabled 0/' \
    -e 's/^-gpgpu_hbf_cache_entries .*/ -gpgpu_hbf_cache_entries 0/' \
    -e 's/^-gpgpu_hbf_buffer_enabled .*/ -gpgpu_hbf_buffer_enabled 0/' \
    -e 's/^-gpgpu_hbf_max_active .*/ -gpgpu_hbf_max_active 8/' \
    -e 's/^-gpgpu_hbf_num_channels .*/ -gpgpu_hbf_num_channels 1/' \
    -e 's/^-gpgpu_hbf_channel_bw_gbps .*/ -gpgpu_hbf_channel_bw_gbps 3.5/' \
    -e 's/^-gpgpu_hbf_write_buffer_entries .*/ -gpgpu_hbf_write_buffer_entries 8/'

# 4. limited_hbf — 完整 HBF（v0.4：4 通道 + 无 GC + 前端放大）
gen limited_hbf \
    -e 's/^-gpgpu_hbf_enabled .*/ -gpgpu_hbf_enabled 1/' \
    -e 's/^-gpgpu_hbf_tR .*/ -gpgpu_hbf_tR 12750/' \
    -e 's/^-gpgpu_hbf_mshr_enabled .*/ -gpgpu_hbf_mshr_enabled 1/' \
    -e 's/^-gpgpu_hbf_cache_entries .*/ -gpgpu_hbf_cache_entries 256/' \
    -e 's/^-gpgpu_hbf_max_active .*/ -gpgpu_hbf_max_active 64/' \
    -e "$FRONTEND_BOOST"

# 5. hbf_ssd_mode — SSD 语义对照（E3：GREEDY GC + 搬移，其余同 limited_hbf）
gen hbf_ssd_mode \
    -e 's/^-gpgpu_hbf_enabled .*/ -gpgpu_hbf_enabled 1/' \
    -e 's/^-gpgpu_hbf_tR .*/ -gpgpu_hbf_tR 12750/' \
    -e 's/^-gpgpu_hbf_mshr_enabled .*/ -gpgpu_hbf_mshr_enabled 1/' \
    -e 's/^-gpgpu_hbf_cache_entries .*/ -gpgpu_hbf_cache_entries 256/' \
    -e 's/^-gpgpu_hbf_max_active .*/ -gpgpu_hbf_max_active 64/' \
    -e 's/^-gpgpu_hbf_media_mode .*/ -gpgpu_hbf_media_mode 1/' \
    -e "$FRONTEND_BOOST"

# 6-10. 通道数扫描（E1：带宽 vs 并行度）
for NCH in 1 2 4 8 16; do
    gen "hbf_ch${NCH}" \
        -e 's/^-gpgpu_hbf_enabled .*/ -gpgpu_hbf_enabled 1/' \
        -e 's/^-gpgpu_hbf_tR .*/ -gpgpu_hbf_tR 12750/' \
        -e 's/^-gpgpu_hbf_mshr_enabled .*/ -gpgpu_hbf_mshr_enabled 1/' \
        -e 's/^-gpgpu_hbf_cache_entries .*/ -gpgpu_hbf_cache_entries 256/' \
        -e 's/^-gpgpu_hbf_max_active .*/ -gpgpu_hbf_max_active 64/' \
        -e "s/^-gpgpu_hbf_num_channels .*/ -gpgpu_hbf_num_channels ${NCH}/" \
        -e "$FRONTEND_BOOST"
done

# 11. hbf_ch_partition — 通道分区映射（E2：权重/KV 分通道，OCP §13.3.3）
gen hbf_ch_partition \
    -e 's/^-gpgpu_hbf_enabled .*/ -gpgpu_hbf_enabled 1/' \
    -e 's/^-gpgpu_hbf_tR .*/ -gpgpu_hbf_tR 12750/' \
    -e 's/^-gpgpu_hbf_mshr_enabled .*/ -gpgpu_hbf_mshr_enabled 1/' \
    -e 's/^-gpgpu_hbf_cache_entries .*/ -gpgpu_hbf_cache_entries 256/' \
    -e 's/^-gpgpu_hbf_max_active .*/ -gpgpu_hbf_max_active 64/' \
    -e 's/^-gpgpu_hbf_channel_map .*/ -gpgpu_hbf_channel_map 1/' \
    -e "$FRONTEND_BOOST"

# 12-15. 消融变体（基于 limited_hbf 单项退化）
gen hbf_nomsgr \
    -e 's/^-gpgpu_hbf_enabled .*/ -gpgpu_hbf_enabled 1/' \
    -e 's/^-gpgpu_hbf_mshr_enabled .*/ -gpgpu_hbf_mshr_enabled 0/' \
    -e "$FRONTEND_BOOST"

# 16. limited_hbf_oldmap — 完整 HBF 但保留旧的 256B 地址交错（对照用）
gen_oldmap limited_hbf_oldmap \
    -e 's/^-gpgpu_hbf_enabled .*/ -gpgpu_hbf_enabled 1/' \
    -e 's/^-gpgpu_hbf_tR .*/ -gpgpu_hbf_tR 12750/' \
    -e 's/^-gpgpu_hbf_mshr_enabled .*/ -gpgpu_hbf_mshr_enabled 1/' \
    -e 's/^-gpgpu_hbf_cache_entries .*/ -gpgpu_hbf_cache_entries 256/' \
    -e 's/^-gpgpu_hbf_max_active .*/ -gpgpu_hbf_max_active 64/' \
    -e "$FRONTEND_BOOST"

gen hbf_nocache \
    -e 's/^-gpgpu_hbf_enabled .*/ -gpgpu_hbf_enabled 1/' \
    -e 's/^-gpgpu_hbf_cache_entries .*/ -gpgpu_hbf_cache_entries 0/' \
    -e "$FRONTEND_BOOST"

gen hbf_nobuffer \
    -e 's/^-gpgpu_hbf_enabled .*/ -gpgpu_hbf_enabled 1/' \
    -e 's/^-gpgpu_hbf_buffer_enabled .*/ -gpgpu_hbf_buffer_enabled 0/' \
    -e "$FRONTEND_BOOST"

gen hbf_serial \
    -e 's/^-gpgpu_hbf_enabled .*/ -gpgpu_hbf_enabled 1/' \
    -e 's/^-gpgpu_hbf_max_active .*/ -gpgpu_hbf_max_active 1/' \
    -e "$FRONTEND_BOOST"

echo "完成。"
