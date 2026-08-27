# HBF-Sim 时序参数验证与时钟域说明

> 本文档回答两个问题：HBF-Sim 的 NAND 时序参数从哪来、单位是什么；
> 以及在没有真实 HBF 样片的情况下，如何对时序模型做组件级验证。
> （论文 §5 Validation Methodology 的支撑材料）

## 1. 时钟域：所有 NAND 计时器按 DRAM 时钟节拍计数

HBF 控制器（`hbf_controller_t`）由 `gpu-sim.cc` 主循环的 **DRAM 时钟域**
驱动（`if (clock_mask & DRAM) { ... hbf_cycle(); }`），与 `dram_t` 完全一致。
因此：

- `-gpgpu_hbf_tR / tPROG / tBERS / buffer_hit_latency / cache_hit_latency`
  的单位都是 **DRAM 时钟节拍（tick）**，不是 GPU core 周期；
- V100 配置（`-gpgpu_clock_domains 1132:1132:1132:850`）下 DRAM 时钟为
  **850 MHz**，1 tick = 1.176 µs；
- 控制器统计里的端到端请求延迟使用 `gpu_sim_cycle` 打时间戳，单位是
  **core 周期**（1.132 GHz）。

**历史错误（v0.3.1 及之前）**：配置注释声称"15 µs @ 1 GHz"，但 tR=15000
按 850 MHz 计数实际是 17.6 µs；03 实验的理想带宽公式又用 1.132 GHz 换算，
三个数字互相矛盾（偏差 ~30%）。**v0.4 起修正为**：配置值 = 目标 µs 数 ×
0.850 GHz，并在统计输出中同时打印 tick 数与 µs 数。

## 2. 默认时序参数（HBF 产品假设档）

| 参数 | HBF-Sim 默认（v0.4） | 物理时间 @850MHz | 注释 |
|---|---|---|---|
| tR（页读） | 12750 ticks | **15 µs** | 读优化的 HBF 级 NAND（"re-architected subarray"假设） |
| tPROG（页编程） | 170000 ticks | **200 µs** | 写非主路径，取激进值；敏感性覆盖 750 µs |
| tBERS（块擦除） | 1700000 ticks | **2 ms** | 同上 |
| 页缓冲命中 | 10 ticks | 11.8 ns | 页寄存器命中（逻辑 die 侧） |
| 页缓存命中 | 50 ticks | 58.8 ns | 逻辑 die SRAM 缓存命中 |

> 依据：OCP HBF v0.7.0 §4.2 将 NAND 时序留给产品 Profile（Table 4 只定义
> 接口带宽档位），公开文献中 HAVEN 采用"re-architected 3D NAND subarrays"
> 的假设，H3 未公布时序。因此 15/200/2000 µs 是**可辩护的产品假设**，
> 论文中必须配敏感性区间（见 §4）。

## 3. 对照档：商品 NAND（MQSim 参数）

MQSim（`MQSim/ssdconfig.xml`，FAST'18 框架的默认参数，来源为商用
TLC/QLC SSD）与本模拟器的对照：

| 参数 | MQSim 默认 | HBF-Sim 默认（HBF 档） | 比值 |
|---|---|---|---|
| Page_Read | 75,000 ns（75 µs） | 15 µs | 5× |
| Page_Program | 750,000 ns（750 µs） | 200 µs | 3.75× |
| Block_Erase | 3,800,000 ns（3.8 ms） | 2 ms | 1.9× |

对比模式（`-gpgpu_hbf_tR 63750 -gpgpu_hbf_tPROG 637500 -gpgpu_hbf_tBERS 3230000`，
即 MQSim 值按 850 MHz 折算）用于论文的敏感性实验：**结论若在两个档位间
定性一致，则对未公开时序稳健**。

## 4. 组件级验证（无样片时代的验证三角）

1. **规范数学上限**：接口带宽 = 通道数 × 256 GB/s × 75%（AXI 链路层效率）
   → 16 通道 = 3.072 TB/s（OCP §4.2 Table 2）。容量 = 16 dies × 16 banks/通道
   × 4 KiB = 512 GiB（Table 3）。模拟器可在"设备独立模式"（规划中）下用
   合成流量验证：`带宽 = min(接口上限, 并行页读数 × 4 KiB / tR)`。
2. **时序交叉验证**：§3 的 MQSim 对照表 + 敏感性扫描（05 实验扩展至
   tPROG/tBERS）。
3. **并行度需求核算**：tR = 15 µs 时，要维持 384 GB/s（Grade 1）需
   `384e9 × 15e-6 / 4096 ≈ 1407` 个并发页读；3.072 TB/s（Grade 3）需约
   `11250` 个并发页读。当前 `max_active=64`（×8 分区 = 512）远低于此，
   这就是论文 §6.1 "带宽悬崖" 的模型侧根源，也是 T6（前端喂流）与
   通道并行度实验的量化基线。

## 5. 与论文实验的对应关系

- 03 权重加载：理想带宽公式改用 DRAM 时钟（0.850 GHz）重算；
- 05 敏感性：tR 扫描 {12750, 63750, 127500}（15/75/150 µs），tPROG 扫描
  {170000, 637500}，tBERS 扫描 {1700000, 3230000}；
- 所有新实验输出带 µs 标注的统计（见 `print_stat` 改动）。
