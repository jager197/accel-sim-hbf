# FAST 模拟器论文写法与 HBF-Sim 创新性评估

> 调研日期：2026-08-21  
> 评估对象：`accel-sim-hbf-dev` 当前工作区（v0.4 开发态）  
> 目标会议：USENIX FAST  
> 结论口径：代码证据、现有实验和公开论文分别核验；未完成的实现或实验不作为既成贡献。

> **状态更新（2026-08-21 同日）**：§3.3 中 "15 passed / 4 failed" 的冒烟失败
> 已修复（根因：v0.4.0 idle-drain 过早 flush 导致同页二次 flush 覆盖在途 MSHR 条目、
> 丢失请求；修复为 flush 即清除缓冲条目 + 在途页条目推迟 flush + 100 tick 安静期）。
> 当前 `FORCE_RERUN=1` 全量冒烟为 **19 passed / 0 failed**，写读回/页编程/块擦除/
> 磨损统计全部通过。v0.4 剩余待办见 `docs/paper_plan.md` 与 §6（E1–E5、验证三角、
> 公平基线、真实 trace）。

## 0. 结论先行

HBF-Sim **有明确的研究创新空间**。需要区分两种 `first`：宽泛的“第一个使用 GPU-HBF
模拟的工作”已经不成立；但“第一个开源、通用、规范驱动的 GPU-HBF 模拟平台”目前仍有较强的
可辩护性。截至 2026-08-21，至少已有以下工作在论文内部使用 GPU-HBF 模型：

- [TileLens](https://arxiv.org/abs/2607.04031) 使用 MacSim 的周期级 GPU 模拟器并加入
  HBF 内存模型；
- [FlashAccel](https://arxiv.org/abs/2607.10186) 在 LLMCompass 上构建事件驱动模拟器，并加入
  plane 粒度 NAND 模型；
- [Beyond Capacity / DASH](https://arxiv.org/abs/2608.14333) 扩展 LLM serving 模拟器，建模
  HBF 页、die/plane 并行和数据路径；
- [HBF Sucks!](https://arxiv.org/abs/2608.11668) 扩展 TokenSim，以生产 trace 做 HBF/SSD
  全栈 KV 层级分析；
- [HAVEN](https://arxiv.org/abs/2603.01175) 对 HBF NAND 子阵列和近存储向量检索单元做详细建模。

这些论文的重点均是架构或系统优化，而不是发布可复用模拟器；目前也没有在论文中提供其 HBF
扩展的公开 artifact。它们会阻止宽泛的“首个 GPU-HBF 模拟”声明，但不会自动否定 HBF-Sim
作为**首个公开研究平台**的贡献。因此，最有竞争力、也最可防守的定位应当是：

> **一个开放、规范驱动、将 OCP HBF 设备语义与 GPU 指令/缓存/内存停顿闭环连接的周期级联合模拟平台；它不仅给 HBF 设置一个延迟和带宽，还能比较 HBF 与 SSD 在通道、页缓冲、写聚合、介质管理和背压语义上的差异。**

当前最强的三个候选贡献是：

1. **GPU-HBF 闭环时序集成**：HBF 排队、NAND 操作和返回背压会改变 GPU kernel 的完成时间，
   而不是在运行结束后用分析公式估算 HBF 时间。
2. **OCP 语义与 SSD 语义的差分建模**：独立 Host Channel、通道资源归属、接口带宽信用、
   4 KiB 写聚合、每通道写并发限制，以及 HBF 模式下关闭 SSD 式 GC。
3. **可解释的跨层设计空间探索**：把 GPU 请求并发度、地址映射、页局部性、通道/子阵列并行度、
   NAND 时序和读写干扰放入同一实验框架，解释“规格带宽为什么没有转化为应用带宽”。

但是，当前工作区还不具备直接投稿所需的证据闭环：现有结果来自 v0.3.1，v0.4 的最新冒烟测试为
**15 passed / 4 failed**；部分 OCP 行为仍是简化或仅计数；现有 workload 和比较基线不足以支撑
“FAST 级”的系统结论。建议把“创新性”和“投稿成熟度”分开判断：

- **研究新颖性：3.8/5，具备投稿潜力；**
- **当前证据成熟度：2.3/5，必须补验证和新实验。**

---

## 1. FAST 中以模拟器/仿真平台为主要贡献的论文

### 1.1 严格意义上的“纯模拟器”

FAST 历史上以模拟器本身为主要贡献的论文并不多。最值得模仿的是 MQSim；FAST '26 的
Xerxes 则提供了一个与 HBF-Sim 更相似的新范式：**新协议/硬件尚不完整可得，通过规范驱动的
组件模型、有限硬件和理论模型进行分层验证，然后用模拟器得出系统设计观察。**

| 论文 | 研究问题 | 模拟对象与粒度 | 可配置性 | 精度验证 | 实验与系统发现 | 复现性 | 对 HBF-Sim 的直接启示 |
|---|---|---|---|---|---|---|---|
| [MQSim, FAST '18](https://www.usenix.org/conference/fast18/presentation/tavakkol) | 旧 SSD 模拟器没有正确覆盖 NVMe 多队列、稳态和端到端延迟 | 模块化离散事件 SSD 模拟器；支持 standalone 和 gem5；覆盖 HIL、FTL、调度、NAND 层级 | 协议、队列、缓存、映射、调度、NAND 几何及时序 | 对比 4 块真实 SSD；不同设备平均误差 6%–18%，真实 trace 平均/最大误差约 11%/18% | 不只证明工具可用，还发现 inter-flow interference 和公平性问题 | 论文发布开源代码、配置和 trace | 用“现有模型的三个可量化错误 → 三个设计修复 → 真实验证 → 新系统发现”的完整论证链 |
| [Xerxes, FAST '26](https://www.usenix.org/conference/fast26/presentation/an) | CXL 3.x 新拓扑、设备管理一致性和 PCIe 6.0 特征缺乏可扩展研究平台 | 面向 CXL 的组合式模拟框架；standalone trace 模式和 gem5 集成；可包装 DRAMsim3、SimpleSSD 等端点模型 | 图结构拓扑、交换机、链路、设备、一致性及物理层参数 | 对真实 CXL 内存平台、理论模型和 SPEC 分层验证；硬件带宽误差 0.1%–10%，loaded-latency 平均误差 4.3%；gem5 集成平均增加约 2% 模拟时间 | 比较链、树、环、spine-leaf、全连接拓扑，研究 DMC 和全双工链路并形成设计观察 | 官方页面给出开源仓库 | HBF 无样片也可以投稿，但必须建立“已有硬件/数学上限/协议不变量/端到端 workload”四层验证链 |

### 1.2 FAST 中值得参考的模拟器相邻论文

这些工作不是纯 simulator，但对工具论文的定位、验证和叙事非常有价值。

| 论文 | 平台类型 | 写作上值得借鉴的点 | HBF-Sim 应避免的误用 |
|---|---|---|---|
| [FEMU, FAST '18](https://www.usenix.org/conference/fast18/presentation/li) | QEMU 全系统闪存 emulator | 用 **CASE**（Cheap, Accurate, Scalable, Extensible）形成易记的设计目标；对 OpenChannel SSD 的差异为 0.5%–38%；支持未修改 OS/应用 | FEMU 的优势是全软件栈执行，不应把 HBF-Sim 的 trace/周期模拟称为全系统 emulator |
| [Cylon, FAST '26](https://www.usenix.org/conference/fast26/presentation/yoon) | 基于 FEMU 的 CXL-SSD 全系统 emulator | 用真实 CXL-SSD 原型验证延迟分布；采用 cache hit 快路径与 NAND miss 慢路径兼顾速度和准确性 | HBF-Sim 尚无样片，不能使用“hardware-validated”或“产品级准确”措辞 |
| [WARP, FAST '26](https://www.usenix.org/conference/fast26/presentation/song) | FDP SSD emulator + 真实设备 characterization | 先用两块商业设备发现现象，再构建可观测 emulator 解释原因，最后用平台探索新策略 | 工具本身不是最终贡献；需要 HBF-Sim 导出的新现象或机制 |
| [Getting Real, FAST '13](https://www.usenix.org/conference/fast13/technical-sessions/presentation/saxena) | 将 SSD 模拟设计迁移到 OpenSSD 原型的经验论文 | 明确指出模拟器容易忽略内存、固件、接口和 OS 栈的真实约束；用硬件实现暴露意外复杂性 | 在没有样片时必须主动列出未建模行为，不能以“周期级”替代真实准确性证明 |

以下工作不应作为“FAST 纯模拟器文章”主样板：FAST '24 Kosmo 的核心是 MRC 生成算法，
虽然使用 miniature simulation，但不是设备/系统模拟平台；FAST '22 WOM-v 使用 FEMU testbed，
模拟平台也不是其主要贡献。

### 1.3 FAST 模拟器论文的共同写法

MQSim 和 Xerxes 的共同结构可压缩为六步：

1. **先证明现有工具错在哪里。** 不写泛泛的“尚无工具”，而是列出 3 个会改变结论的缺口。
2. **把缺口映射到模型组件。** 每个组件都对应一个 observable 或可验证不变量。
3. **先验证，后做 case study。** Validation 独立成节，不能把“结果看起来合理”当验证。
4. **同时报告精度和速度。** 工具论文要回答为何不用更简单或更详细的模型。
5. **用模拟器产生新认识。** MQSim 有流间干扰，Xerxes 有拓扑/全双工/一致性观察。
6. **公开代码、配置和 workload。** artifact 是工具价值的一部分，而不是附属材料。

HBF-Sim 的 Introduction 不宜以“没有开源 HBF 模拟器”作为唯一动机，而应明确三类错误：

- 把 HBF 当固定延迟大内存，会忽略 NAND page、页缓冲、写聚合和子阵列冲突；
- 把 HBF 当 NVMe SSD，会错误引入 NVMe 队列、设备侧 GC 和有效页搬移；
- 用 LLM 分析模型，会忽略 GPU 发射/缓存/在飞请求限制与 HBF 排队之间的闭环反馈。

---

## 2. 与 2026 年 HBF 研究的重叠和差异

### 2.1 最直接的相关工作

| 工作 | 模拟方法 | 已覆盖内容 | 与 HBF-Sim 的重叠 | HBF-Sim 可建立的差异 |
|---|---|---|---|---|
| [H³, IEEE CAL 2026](https://doi.org/10.1109/LCA.2026.3660969) | 面向 LLM 的内部分析模型 | HBM+HBF 容量、带宽、功耗、batch 和 latency-hiding buffer | HBM/HBF 混合层级、权重/共享 KV 场景 | HBF-Sim 可提供请求级排队、页行为和 GPU 停顿反馈；不能只重复其容量收益 |
| [HAVEN](https://arxiv.org/abs/2603.01175) | 3D-FPIM + NeuroSim NAND/电路模型，叠加 ANNS pipeline | 子阵列重构、功耗约束、近存储 reranking | NAND 子阵列并行和 HBF 带宽 | HBF-Sim 更偏通用 GPU 执行和 OCP 接口语义；HAVEN 在物理/电路建模上更深 |
| [TileLens](https://arxiv.org/abs/2607.04031) | MacSim 周期级 GPU + HBF/RoMe 模型，SASS trace | 4 KiB 粒度、读放大、混合粒度 L2、MSHR、预取 | 周期级 GPU-HBF 联合模拟，与 HBF-Sim 最直接重叠 | HBF-Sim 必须强调通用可配置平台、OCP Host Channel/写路径/无 GC 语义和开放 artifact，而不是“首次周期级” |
| [FlashAccel](https://arxiv.org/abs/2607.10186) | LLMCompass 事件驱动模型 + plane 粒度 NAND simulator | 权重/KV 布局、预取、SRAM、LLM 端到端吞吐 | NAND 延迟/并行及 LLM workload | HBF-Sim 的优势应是 kernel/缓存闭环和非特定应用；FlashAccel 的 LLM 端到端指标更完整 |
| [HBF Sucks!](https://arxiv.org/abs/2608.11668) | 扩展 TokenSim + 生产 trace + 热/耐久模型 | 完整 serving 调度、KV 读写比、SLO goodput、热和寿命 | HBF/SSD 层级和 KV 场景 | HBF-Sim 需要设备微结构和 GPU 闭环；同时必须补 thermal/endurance 或明确不覆盖 |
| [Beyond Capacity / DASH](https://arxiv.org/abs/2608.14333) | LLMSimulator + HBF die/plane/page/链路模型 + GPU 实测算子 | MoE/KV 数据路径、直接/中继链路、端到端吞吐 | HBF 并行和 GPU 数据路径 | HBF-Sim 可做更细的请求级控制器研究，但需增加真实模型 trace 和 serving 指标 |
| [Potential Applications of HBF](https://arxiv.org/abs/2608.13127) | 事件驱动 LLM serving 模型；把 HBF 抽象为带宽保持的额外容量 | MoE expert 复制、多模型驻留 | 容量收益和 LLM 场景 | HBF-Sim 可检验其“带宽保持”假设何时成立 |
| [CXL-MQSim / ATC '23](https://www.usenix.org/conference/atc23/presentation/yang-shao-peng) | memory trace + MQSim 衍生的 CXL-flash 离散事件模型 | load/store 类闪存扩展、cache/prefetch、NAND 内部 | 内存语义闪存和 trace 驱动设计空间 | 它是最重要的非 HBF 基线；HBF-Sim 需解释 OCP HBF 与 CXL-flash 的接口和介质差异 |

### 2.2 “first” 声明的安全边界

下列表述已经不安全：

- “the first GPU-HBF simulator”；
- “the first cycle-level evaluation of HBF-augmented GPUs”；
- “the first model of HBF page-level behavior”。

原因不是相关论文以模拟器为主要贡献，而是它们已经实际构建并使用了 GPU-HBF 模型。审稿人会把
“first GPU-HBF simulator”理解为此前从未存在过此类模型，而不限定是否开源或是否为论文主题。

### 2.3 公开 artifact 状态核验

截至 2026-08-21，对论文正文和公开仓库的检查结果是：

- TileLens 论文说明其修改 MacSim，但没有给出 TileLens/HBF 模型仓库；
- FlashAccel 论文说明其在 LLMCompass 上加入 NAND simulator，但没有给出该扩展代码；
- HAVEN 没有给出可复用的 GPU-HBF 模拟器代码；
- DASH 引用了公开的 `scale-snu/LLMSimulator`，但其公开 `main` 分支中没有 HBF/DASH 实现；
- HBF Sucks! 引用了公开的 `pku-lemonade/TokenSim`，但其公开 `main` 分支中没有论文的 HBF 扩展；
- H³ 明确使用内部分析模拟器，没有公开平台。

因此，目前更有价值的 `first` 不是“第一次做过 HBF simulation”，而是：

> To the best of our knowledge, HBF-Sim is the first **open-source, general-purpose,
> OCP-specification-driven, cycle-level GPU–HBF simulation framework**.

其中 `open-source` 表示论文使用的 HBF 模型和复现实验都实际发布；`general-purpose` 表示平台可运行
不同 CUDA/SASS workload，而不是只编码某个 LLM/ANNS 数据流。提交前仍需再次检查同期论文和 artifact，
并保证匿名 artifact 可以从干净环境复现。

在完成代码发布和完整 related-work 复核后，可考虑使用更窄的限定：

> To the best of our knowledge, HBF-Sim is the first **open, OCP-specification-driven,
> cycle-level GPU–HBF co-simulation platform** that exposes both GPU execution feedback
> and HBF/SSD media-semantic differences.

这句话成立需要同时满足：

1. artifact 能从干净环境构建并运行；
2. 论文逐项列出“specification-driven”实际覆盖的规范语义；
3. 不把未实现的 AXI ordering、错误响应、热/功耗、ECC 等包含在“faithful”范围内；
4. 明确 TileLens 和 FlashAccel 已经具有 GPU-HBF 模拟能力，但用途和公开程度不同。

---

## 3. 当前仓库的实现证据

### 3.1 已经形成的核心能力

| 能力 | 代码证据 | 可支持的论文表述 |
|---|---|---|
| GPU-HBF 闭环 | `gpu-simulator/gpgpu-sim/src/gpgpu-sim/l2cache.cc::hbf_cycle()`；HBF 请求从 L2/DRAM 队列进入控制器，完成后返回 L2/SM；`gpu-sim.cc` 在 DRAM 时钟域逐拍调用 | HBF 排队和完成时间直接影响 GPU kernel 周期 |
| 地址空间共存 | `memory_config::is_hbf_addr()` 和 L2 路由；HBF 地址保留 64 位并绕过会改写地址的 L2 MSHR 路径 | 同一 GPU 地址空间内可配置 HBM/DRAM 与 HBF 区域 |
| NAND 时序和并行 | `hbf_subarray_t` 的 READ/PROGRAM/ERASE 状态机；`hbf_controller_t::schedule_operations()`；`hbf_max_active` | 可扫描 tR/tPROG/tBERS、子阵列冲突和功率约束并行度 |
| Host Channel | `hbf_channel.h`；`hbf_num_channels`、`hbf_channel_map`、每通道资源切片、带宽信用和写并发计数 | 可研究通道扩展、地址交错/分区及接口瓶颈 |
| 粒度桥接 | page-keyed MSHR、共享 page cache、多 page buffer、64 B GPU 请求到 4 KiB NAND page 的合并 | 可量化页读次数、合并率、buffer/cache 命中和读放大 |
| 写路径 | write buffer、aggregation timeout、erase/program、per-channel outstanding write limit | 可研究小写入聚合、写背压和读写干扰 |
| HBF/SSD 差分模式 | `hbf_media_mode`；HBF 模式关闭 SSD 式 GC，SSD 模式保留 GREEDY GC；zone remap 统计 | 可直接证明“复用 SSD 模型会产生什么偏差” |
| 可观测性 | 请求/页读/编程/擦除、MSHR、cache/buffer、通道 stall、容量和 FTL 统计 | 已具备工具论文需要的多层指标基础 |
| 仿真速度优化 | 活跃子阵列集合代替每周期扫描 16,384 个子阵列；历史记录约从 1.5K 提升到 50K cycles/s | 可作为工程贡献，但必须重新做可复现实测 |

### 3.2 现有结果可以支持什么

现有 v0.3.1 结果能支持的只是**候选观察**，不能直接作为 v0.4 论文结果：

- 页级地址交错把 16 MiB 顺序读的 NAND 页读次数从 32,768 降至 4,096，即消除
  8× 内部重复页读；但应用带宽只从 13.3 提升到 14.4 GB/s，说明 GPU 前端在飞请求限制
  掩盖了介质侧优化。这是一个有价值的跨层现象。
- 顺序流实测仅 12.7–14.4 GB/s，而旧层级模型给出约 142–158 GB/s 的介质并行上限，
  暴露“接口峰值—NAND 服务能力—GPU 喂流能力”三者之间的断层。
- KV 外溢实验显示 HBF 比人为设置的串行慢层快 57×–186×，但该基线不等价于真实 NVMe、
  CXL-flash 或 SSD，数字不适合作为论文主结论。
- KV 写实验显示批量可摊薄固定开销，但 HBF 写仍比 DRAM 慢两个数量级；该趋势与 2026 年
  HBF 相关研究一致，但需要用 v0.4 的 4 KiB 聚合和 no-GC 语义重跑。

### 3.3 当前阻止投稿的事实

1. `experiments/01_smoke/results/smoke_report.txt` 最新结果是 **15 passed / 4 failed**，
   写读回、page program、block erase 和磨损统计未通过；旧 `SUMMARY.md` 中的 19/19 是
   v0.3.1 结果，不能代表当前 v0.4。
2. v0.4 的 Host Channel、no-GC、容量和新写路径没有对应的正式 E1–E5 结果；现有 CSV
   仍是旧模型或从旧转录恢复的数据。
3. `setup_hbf.sh` 当前只显式复制 `hbf.h` 和 `hbf.cc`，没有复制
   `hbf_controller.*`、`hbf_ftl.*`、`hbf_page_cache.*`、`hbf_subarray.*` 和
   `hbf_channel.h`；从干净 Accel-Sim 环境安装可能无法复现当前源码树。
4. timeout 路径当前会“执行 partial-page program 并计数”，而代码注释本身指出 OCP 行为
   应是错误响应；论文不能把它写成已符合规范的行为。
5. zone remapping 当前只计数，没有执行数据/映射变化；capacity exhaustion 路径会丢弃 victim
   的有效 mapping，虽声明 workload 不会触发，但必须改为显式失败或证明不可达。
6. 接口信用主要限制读返回数据，写入数据的链路占用、AXI ID ordering、non-posted completion、
   错误响应和跨 channel 异步时钟尚未完整建模。
7. 仍缺 ECC、read retry、bad-block、refresh、功耗、热限制和耐久模型；而 2026 年相关论文已经
   把 thermal/endurance 作为 HBF 结论的重要边界。

---

## 4. 创新性五维评分

| 维度 | 评分 | 现有证据 | 主要扣分项 | 论文中的正确定位 |
|---|---:|---|---|---|
| 模拟对象创新 | 4.0/5 | OCP HBF 是新设备；开源平台稀缺；具备 HBF/SSD 语义差分 | TileLens、FlashAccel 等已包含 HBF 模型 | 不称“首个 HBF 模拟器”，强调开放、规范驱动和通用控制器研究 |
| 跨层建模创新 | 4.5/5 | GPU kernel、cache/互连、地址路由、HBF 排队和 NAND 时序闭环 | HBF 请求绕过 L2，真实 mixed-granularity cache 交互被简化；缺 serving runtime | 这是最强主贡献，应画完整请求/背压/完成路径 |
| 粒度—速度折中 | 3.0/5 | 周期级 GPU + 事件式活跃子阵列；已有约 30× 工程优化线索 | 尚无系统化精度/速度曲线，无更详细模型作参照 | 报告每秒模拟周期、内存、相对 Accel-Sim 开销和模型消融 |
| 设计空间探索 | 4.0/5 | 通道、映射、并行度、NAND 时序、cache/buffer、HBF/SSD mode 均可配置 | 新参数尚未形成自动化 Pareto 和真实 workload 结果 | 以“接口—介质—GPU 前端”三层瓶颈图为主线 |
| 新系统发现 | 2.5/5 | 已观察到页读减少未等比例转化为带宽、前端喂流瓶颈和写不对称 | workload 合成且规模小，外部慢层基线过弱，旧结果不对应 v0.4 | 需要 E1–E5 和 case study 才能成为 FAST 贡献 |

### 综合判断

- **有创新点**：尤其是 OCP 语义差分和 GPU-HBF 闭环；
- **创新点不应是代码功能列表**：FTL、MSHR、cache、page buffer 单独都不是新机制；
- **论文成败取决于发现**：必须证明一个简单 HBF/SSD/固定延迟模型会给出错误的系统结论，
  再展示 HBF-Sim 如何定位原因并支持新机制。

---

## 5. 推荐的论文主张与标题

### 5.1 推荐标题

首选：

> **HBF-Sim: Spec-Driven, Cycle-Level Co-Simulation of GPUs and High-Bandwidth Flash**

如果完成 HBF/SSD 差分实验，可用更强的问题导向标题：

> **HBF Is Not an SSD: Modeling OCP High-Bandwidth Flash in a Cycle-Level GPU Simulator**

当前 docx 中的 “GPU Full-System Simulator” 不够准确。Accel-Sim 的 trace-driven 模式不是运行
完整 OS 的 full-system simulator；建议使用 `GPU microarchitecture simulator`、
`cycle-level GPU simulator` 或 `GPU–HBF co-simulation platform`。

### 5.2 推荐的三条贡献

在完成下述验证后，Introduction 可写成：

1. **A spec-driven GPU–HBF co-simulation platform.** We build HBF-Sim on
   Accel-Sim/GPGPU-Sim and connect GPU memory requests, cache/interconnect backpressure,
   HBF controller queues, and NAND operations in a common cycle-level timing loop.
2. **An explicit model of HBF semantics rather than SSD behavior.** HBF-Sim models
   channel-affine resources, bounded host-channel bandwidth, page-buffered reads,
   4-KiB write aggregation, and HBF media management without device-side SSD garbage
   collection; a paired SSD mode quantifies errors caused by reusing SSD abstractions.
3. **A cross-layer characterization and controller case study.** Across real GPU traces
   and controlled microbenchmarks, we identify when interface bandwidth is limited by
   NAND parallelism, address mapping, request concurrency, and writes, and use these
   findings to design and evaluate page coalescing plus write-isolation scheduling.

其中第 3 条必须替换为最终实测发现和数字；没有 E1–E5 结果时不要提前写 speedup。

### 5.3 不应作为核心创新的内容

- “实现了 MSHR / LRU cache / FTL / wear leveling”；这些都是常见组件；
- “配置项很多”；可配置性是工具属性，不是独立系统贡献；
- “KV 外溢比串行慢层快 186×”；基线过弱且结果来自旧版；
- “HBF 带宽接近 HBM”；当前实测不支持，且公开研究也表明需要布局和预取等条件；
- “模型高保真”；没有 HBF 硬件或 RTL 验证时只能说 `specification-constrained`、
  `cycle-level` 或 `component-validated`。

---

## 6. 必须补齐的验证和实验

### P0：先使 artifact 和语义可信

1. 修复当前 4 个 smoke failure，并冻结 v0.4 tag；旧 CSV 不混入新论文结果。
2. 在干净容器执行 `setup_hbf.sh → build → smoke`，补全所有源文件复制/patch，并记录 commit、
   CUDA、编译器和随机种子。
3. 将 4 KiB 写聚合 timeout 实现为规范定义的错误/拒绝路径，或明确声明为研究性 partial-write
   扩展；不能一边称 spec-faithful 一边偷偷 program partial page。
4. capacity exhaustion 不得静默丢有效映射；测试模式下应 fail-fast 并输出明确错误。
5. 把 zone remapping 标成 `accounting-only`，除非真正修改映射并建模迁移/host 开销。
6. 为 channel affinity、no-GC、write limit、page boundary 和返回队列背压写独立单元/微基准测试。

### P1：建立“无样片时代”的验证三角

| 验证 | 方法 | 接受标准 |
|---|---|---|
| 规范不变量 | 1/2/4/8/16 channel 合成流；页边界、通道归属、写聚合、no-GC、bounded capacity | 每个不变量自动 PASS；无跨 channel 资源使用；HBF mode 的 GC/valid-page move 为 0 |
| 数学上限 | 独立 traffic generator 饱和控制器；比较 `min(link BW, active×4KiB/tR)` | 稳态误差建议 ≤5%；解释 warm-up、credit 和队列造成的差异 |
| NAND 时序交叉验证 | 用 MQSim 的 75/750/3800 µs profile，在等价单通道/无 GC 设置下比较 read/program/erase | 单操作延迟接近配置值；吞吐趋势和并行扩展一致 |
| GPU 基线继承 | 未修改 Accel-Sim 与 HBF-Sim 在纯 DRAM 配置下运行同一 trace | GPU 周期和关键统计一致，证明 HBF patch 未污染 DRAM 路径 |
| 独立 reference model | Python/C++ 小模型检查页映射、聚合、顺序 program、buffer replacement | 请求序列和完成顺序逐事件一致 |
| 模拟速度 | 与 vanilla Accel-Sim、旧全扫描版本比较 wall time、RSS、sim cycles/s | 报告几何均值；不能只引用历史手工观察的 30× |

### P2：FAST 主实验

1. **E1：通道/并行缩放。** channel={1,2,4,8,16} × max_active × tR，使用独立流量发生器和
   GPU trace 两条曲线，区分接口上限、NAND 上限和 GPU 喂流上限。
2. **E2：地址映射与 workload 匹配。** interleave、partition、热点/偏斜映射；权重、共享 KV、
   临时 KV、随机向量四类 workload；报告 per-channel imbalance、page reads 和 P99。
3. **E3：HBF 不是 SSD。** 同一请求流运行 `hbf_media_mode=0/1`，比较 GC traffic、有效页搬移、
   写放大、前台尾延迟和寿命 proxy。这应成为论文最有辨识度的一张图。
4. **E4：读写干扰与调度 case study。** FCFS、read-priority、write-drain、静态通道隔离；扫描
   写比例、写缓冲水位和最大等待时间，报告 throughput、P50/P99 和 starvation。
5. **E5：精度—速度与参数敏感性。** HBF profile 和商品 NAND profile；tR/tPROG/tBERS、cache、
   page buffer、MSHR、link credit 消融；报告定性结论是否跨 profile 保持。

### P3：端到端 workload 和公平基线

- 至少采集 3 类真实 SASS trace：GEMM/权重流、attention/KV、ANN/random gather；合成 kernel
  只用于控制变量，不作为唯一结果。
- 基线至少包括 HBM-only、PCIe/NVMe、CXL-flash/MQSim profile、简单 fixed-latency HBF 和
  完整 HBF-Sim；不能只用“串行 20K-cycle spill tier”。
- 系统指标至少包含 kernel cycles、effective bandwidth、page amplification、P99、通道利用率和
  simulation throughput。若讨论 serving，需要另接 TokenSim/LLMSimulator 或明确只报告 kernel 层。
- 所有 projected 结果与 measured 结果分图、分表，不能混画。

---

## 7. 可直接套用的 FAST 论文结构

### §1 Introduction

用四段而不是功能列表：

1. HBM 容量压力和 HBF 机会；
2. HBF 不是大容量 DRAM，也不是 NVMe SSD；
3. 三类现有模型为何会错（固定延迟、SSD 语义、高层分析）；
4. HBF-Sim、验证方法、三个定量发现和贡献。

Intro 中必须出现一个能被审稿人记住的矛盾，例如：

> Reducing internal NAND reads by 8× improves application bandwidth by only X%, because
> the GPU front end cannot expose enough outstanding page accesses.

这个句式比“我们支持 16 个 channel 和 30 个参数”更像 FAST 系统论文。

### §2 Background and Model Gaps

- OCP HBF 的最小必要语义；
- 与 HBM、CXL-flash、NVMe SSD 的区别；
- 对 TileLens、FlashAccel、H³、MQSim、Xerxes 的能力矩阵；
- 用 2–3 个反例证明简化模型会产生错误结论。

### §3 HBF-Sim Design

- 请求路径和时钟域；
- Host Channel/链路/资源归属；
- page buffer、MSHR、NAND 状态机和写聚合；
- HBF/SSD media mode；
- 配置、统计和扩展点；
- 明确不建模项。

### §4 Validation

按 MQSim/Xerxes 的顺序写：

1. DRAM regression；
2. 规范不变量；
3. 数学上限；
4. MQSim profile 交叉验证；
5. 端到端 trace sanity；
6. 仿真速度和内存开销。

### §5 Characterization

围绕三个 RQ：

- RQ1：什么时候接口峰值能转化为有效带宽？
- RQ2：GPU 粒度、布局和并发如何造成 page amplification 和通道失衡？
- RQ3：HBF 写路径与 SSD 语义差异如何影响读写混合 workload？

### §6 Case Study

由 §5 的现象导出 page coalescing + write-isolation/write-drain；必须与简单策略对比，并报告尾延迟
和 starvation，而不只是平均带宽。

### §7 Discussion and Limitations

主动写明：无样片、OCP/产品参数演进、没有 ECC/read retry/thermal/endurance、GPU trace 规模限制、
page cache 和 NAND profile 的假设。透明的边界比模糊的“高保真”更可信。

### §8 Related Work / §9 Conclusion

Related Work 分为 GPU 模拟器、SSD/CXL 模拟器、HBF 架构/系统三组。Conclusion 只重述经过实验
证明的结论，不重述配置项。

---

## 8. 摘要与 Introduction 贡献表述草案

### 8.1 英文摘要草案

> High-bandwidth flash (HBF) has emerged as a capacity-oriented, package-local memory
> tier for accelerators. Its wide interface, however, does not make HBF a larger form of
> DRAM: GPU requests interact with channel-affine NAND resources, page-buffered access,
> microsecond media latency, and asymmetric writes. Existing HBF studies primarily rely
> on application-specific analytical or event-driven models, while SSD simulators impose
> host-interface and media-management behaviors that do not match the OCP HBF model.
> We present **HBF-Sim**, an open, cycle-level GPU–HBF co-simulation platform built on
> Accel-Sim/GPGPU-Sim. HBF-Sim connects GPU execution and cache/interconnect backpressure
> to a specification-driven HBF controller with independent host channels, bounded link
> bandwidth, page-buffered NAND operations, 4-KiB write aggregation, and an explicit
> HBF mode without device-side SSD garbage collection. We validate HBF-Sim using GPU
> regression tests, specification invariants, analytical bandwidth bounds, and NAND timing
> profiles cross-checked against MQSim. Across **[WORKLOADS]**, HBF-Sim reveals that
> **[FINDING 1]**, **[FINDING 2]**, and **[FINDING 3]**. Guided by these observations, we
> implement **[MECHANISM]**, improving **[METRIC]** by **[X]** while bounding **[TAIL/WRITE
> COST]**. HBF-Sim provides a reproducible platform for exploring HBF controllers, data
> placement, and GPU memory hierarchies before production HBF hardware is available.

方括号必须用 v0.4 正式结果替换。若没有 MQSim 交叉验证，应删除对应句子而不是保留为计划。

### 8.2 Introduction 贡献段落草案

> This paper makes three contributions. First, we develop HBF-Sim, an open cycle-level
> co-simulation platform that closes the timing loop between GPU execution and HBF device
> behavior. Second, we derive an HBF controller model from the OCP specification and make
> its semantic differences from SSDs explicit, including channel-affine resources, bounded
> host-channel bandwidth, page-oriented writes, and the absence of device-side SSD garbage
> collection. Third, we use HBF-Sim to characterize the joint effects of GPU request
> concurrency, address mapping, NAND parallelism, and writes, and demonstrate a controller
> mechanism derived from the resulting bottlenecks.

### 8.3 一句话定位

中文：

> HBF-Sim 的核心不是“给 Accel-Sim 加一块闪存”，而是让 GPU 发出的细粒度请求、HBF 的
> 规范语义和 NAND 内部并行在同一个时序闭环中相互反馈。

英文：

> HBF-Sim is not a fixed-latency flash attachment to Accel-Sim; it is a timing-closed,
> specification-driven GPU–HBF co-simulation platform.

---

## 9. 最终投稿判定门槛

满足以下条件后，HBF-Sim 才适合以“模拟器论文”而不是“工程实现”投稿 FAST：

- v0.4 correctness 全部通过，clean-build artifact 可复现；
- 至少三层验证：规范/数学、组件交叉、端到端 trace；
- 报告精度—速度折中；
- 明确优于 fixed-latency、SSD-derived 和高层分析模型的场景；
- 至少三个非平凡系统发现，其中一个由 HBF/SSD 语义差分产生；
- 一个从发现自然导出的机制 case study；
- 真实 GPU trace 和公平外部层级基线；
- 不使用未经证明的 `first`、`full-system`、`high-fidelity` 或“接近 HBM 带宽”等表述。

如果只能完成模型和微基准，应把目标收缩为模拟器/建模 workshop 或架构短文；如果 E1–E5、
验证和 case study 全部完成，则主线具备 FAST 长文潜力。

---

## 参考资料

### FAST / USENIX

- [MQSim: A Framework for Enabling Realistic Studies of Modern Multi-Queue SSD Devices, FAST '18](https://www.usenix.org/conference/fast18/presentation/tavakkol)
- [The CASE of FEMU, FAST '18](https://www.usenix.org/conference/fast18/presentation/li)
- [Xerxes: Extensive Exploration of Scalable Hardware Systems with CXL-Based Simulation Framework, FAST '26](https://www.usenix.org/conference/fast26/presentation/an)
- [Cylon: Fast and Accurate Full-System Emulation of CXL-SSDs, FAST '26](https://www.usenix.org/conference/fast26/presentation/yoon)
- [Characterizing and Emulating FDP SSDs with WARP, FAST '26](https://www.usenix.org/conference/fast26/presentation/song)
- [Getting Real: Lessons in Transitioning Research Simulations into Hardware Systems, FAST '13](https://www.usenix.org/conference/fast13/technical-sessions/presentation/saxena)
- [Overcoming the Memory Wall with CXL-Enabled SSDs, USENIX ATC '23](https://www.usenix.org/conference/atc23/presentation/yang-shao-peng)

### 模拟器与 HBF 相关工作

- [Accel-Sim: An Extensible Simulation Framework for Validated GPU Modeling, ISCA '20](https://doi.org/10.1109/ISCA45697.2020.00047)
- [SimpleSSD: Modeling Solid State Drives for Holistic System Simulation](https://doi.org/10.1109/LCA.2017.2750658)
- [NANDFlashSim: Intrinsic Latency Variation Aware NAND Flash Memory System Modeling](https://doi.org/10.1109/MSST.2012.6232389)
- [H³: Hybrid Architecture Using HBM and HBF for Cost-Efficient LLM Inference](https://doi.org/10.1109/LCA.2026.3660969)
- [HAVEN](https://arxiv.org/abs/2603.01175)
- [TileLens](https://arxiv.org/abs/2607.04031)
- [FlashAccel](https://arxiv.org/abs/2607.10186)
- [HBF Sucks!](https://arxiv.org/abs/2608.11668)
- [Beyond Capacity / DASH](https://arxiv.org/abs/2608.14333)
- [Potential Applications of HBF in LLM Serving Systems](https://arxiv.org/abs/2608.13127)
