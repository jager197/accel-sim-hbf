# HBF-Sim

HBF-Sim 是集成于 Accel-Sim/GPGPU-Sim 的规格驱动 GPU/高带宽闪存模拟器，
建模页粒度请求合并、通道和子阵列亲和、页缓冲、logic-die cache、完整页写入、
控制器调度、有限准入以及请求/完成 trace。

公开仓库包含模拟器、固定集成输入、精简 trace、运行脚本和论文使用的数值汇总。
论文源码与 `docs/` 单独维护，并明确排除在仓库之外；生成的运行目录不会提交。

## 快速开始

需要 Linux、Bash、Git、GNU Make、CMake、C++ 编译器、Python 3.10+ 以及带
`nvcc` 的 CUDA 11/12；运行模拟器不需要物理 NVIDIA GPU。

```bash
python3 -m venv .venv && source .venv/bin/activate
python -m pip install -r requirements.txt
export CUDA_INSTALL_PATH=/usr/local/cuda
make bootstrap
make build JOBS="$(nproc)"
make test
make artifact-check
```

## 复现论文证据

```bash
make paper-plan
make setup-mqsim
RUN_TAG=paper-001 make reproduce-paper
```

默认 `core` 套件覆盖功能验证、外部 trace、MSHR/MQSim 对照、介质扩展、页面放置和热点映射、
读写隔离以及成本/容量检查。可用 `SUITE=validation|trace|media|placement|hotspots|isolation|cost|comparison`
选择单个套件；MQSim 对照需先执行 `make setup-mqsim`。

Qwen3-1.7B 完整回放需要验证 trace 包：

```bash
make prepare-qwen
SUITE=qwen RUN_TAG=qwen-001 make reproduce-paper
```

所有运行都会保存命令、配置和二进制哈希、日志、trace 验证报告及退出码；已有 tag 不可复用。

完整步骤见[制品评测指南](artifact/ARTIFACT_EVALUATION.md)，实际验证范围见
[发布验证记录](artifact/VERIFICATION.md)。

## 目录

- `hbf/`：维护中的 HBF 模型和单元测试。
- `gpu-simulator/`：Accel-Sim/GPGPU-Sim 集成树。
- `artifact/`：固定 revision、配置、parser 快照、参考数据和 trace 输入。
- `experiments/`：精选复现实验；废弃开发实验仅保留在本地。
- `util/traces/`：精确 `hbf-trace-v1` 验证和地址重映射工具。
- `scripts/`：引导、构建、测试、打包和复现脚本。

`make figures` 可根据 `artifact/reference/` 中的 CSV 重新生成图。模型通过组件级验证；
时序和带宽参数是研究假设，不代表硅片周期精确性。
