# HBF-Sim

[English](README.md) | [简体中文](README_CN.md)

HBF-Sim 是基于 Accel-Sim/GPGPU-Sim 的可配置 GPU–高带宽闪存模拟器。
它将 GPU 执行与 NAND 页服务连接起来，用于研究内存放置、请求合并、
控制器调度和闪存并行度对 GPU 性能的影响。

## 功能

- **GPU–HBF 闭环反馈**：支持执行驱动的 CUDA 负载和 trace 驱动的回放，
  准入压力与请求完成会反馈到 GPU 执行过程。
- **可配置拓扑**：支持独立 HBF stack、通道、通道亲和子阵列，以及介质操作并发上限。
- **页粒度访问**：支持页级 MSHR 合并、NAND 页缓冲、logic-die cache 和完整页写聚合。
- **策略探索**：支持地址映射、按需/聚合读取，以及 FCFS、读优先和有界写排空调度。
- **请求追踪**：提供请求生命周期事件和验证工具，用于分析延迟、流量与完成顺序。

模型遵循 OCP HBF 接口规范，NAND 时序和链路参数均可配置。
模型验证情况及支持范围见[验证说明](artifact/VERIFICATION.md)。

## 安装

需要 Linux、Bash、Git、GNU Make、CMake、C++ 编译器、zlib 开发头文件、
`makedepend`（`xutils-dev`）、Python 3.10+，以及带 `nvcc` 的 CUDA 11/12。
运行模拟器不需要物理 NVIDIA GPU；采集新的应用 trace 则需要。

```bash
git clone --branch dev https://github.com/jager197/accel-sim-hbf.git
cd accel-sim-hbf

python3 -m venv .venv
source .venv/bin/activate
python -m pip install -r requirements.txt
export CUDA_INSTALL_PATH=/usr/local/cuda

make bootstrap
make build JOBS=4
make test
```

`make bootstrap` 会在固定版本的 GPGPU-Sim 上安装 HBF 扩展。
依赖详情和 CUDA 开发容器的使用方法见[安装指南](artifact/README.md)。

## 使用示例

运行随附的功能示例：

```bash
make reproduce-smoke RUN_TAG=smoke-001
```

这会检查 DRAM 数值基线、HBF 通道路由和同页读合并。
也可以通过 GPU–HBF 路径回放随附的 LUD trace：

```bash
make trace-smoke RUN_TAG=trace-001
```

每个示例会将配置、日志和请求 trace 写入对应的 `experiments/` 结果目录。
每次运行请使用新的 `RUN_TAG`。

[HBF 配置文件](hbf/gpgpusim_hbf.config) 说明了拓扑、介质时序、缓存、映射和调度选项。
更多负载与分析命令见[实验指南](experiments/README.md)，主要入口可通过 `make help` 查看。

## 目录结构

| 目录 | 内容 |
|---|---|
| `hbf/` | HBF 模型、配置、集成补丁和单元测试 |
| `gpu-simulator/` | Accel-Sim 前端与 GPGPU-Sim 集成 |
| `experiments/` | 示例负载和实验运行脚本 |
| `artifact/` | 构建输入、示例 trace、参考数据和指南 |
| `util/traces/` | Trace 验证与地址重映射 |
| `scripts/` | 安装、构建和验证脚本 |

## 文档

- [安装与构建](artifact/README.md)
- [实验与负载](experiments/README.md)
- [详细评测及 Qwen 采集/回放](artifact/ARTIFACT_EVALUATION.md)
- [验证说明](artifact/VERIFICATION.md)

## 致谢与许可证

HBF-Sim 基于 [Accel-Sim](https://github.com/accel-sim/accel-sim-framework)
和 [GPGPU-Sim](https://github.com/accel-sim/gpgpu-sim_distribution) 构建，
HBF 建模参考 [OCP HBF 规范](https://www.opencompute.org/documents/ocp-hbf-architecture-specification-v0-7-0-final-pdf)。
仓库许可证与保留的版权声明见 [LICENSE](LICENSE)。
