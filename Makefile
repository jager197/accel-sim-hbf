SHELL := /bin/bash
JOBS ?= 4
PYTHON ?= python3
RUN_TAG ?=
MAX_PAR ?= 1
SUITE ?= core
.DEFAULT_GOAL := help
.PHONY: help bootstrap bootstrap-check build test test-tools artifact-check reproduce-smoke trace-smoke paper-plan reproduce-paper setup-mqsim figures container
help:
	@printf '%s\n' 'HBF-Sim:' \
	 '  make bootstrap / build / test / artifact-check' \
	 '  make test-tools          checks requiring Python only' \
	 '  make paper-plan          current experiment coverage and commands' \
	 '  make setup-mqsim         install pinned external comparison dependency' \
	 '  make reproduce-smoke RUN_TAG=smoke-001' \
	 '  make reproduce-paper RUN_TAG=run-001 SUITE=core MAX_PAR=1' \
	 '  make figures             plot released reference summaries' \
	 '  make container           build CUDA development image'
bootstrap:
	@CUDA_INSTALL_PATH="$${CUDA_INSTALL_PATH:-/usr/local/cuda}" scripts/bootstrap.sh
bootstrap-check artifact-check:
	@scripts/bootstrap.sh --check-only
build:
	@CUDA_INSTALL_PATH="$${CUDA_INSTALL_PATH:-/usr/local/cuda}" JOBS="$(JOBS)" scripts/build.sh
test:
	@PYTHON="$(PYTHON)" scripts/test.sh
test-tools:
	@PYTHON="$(PYTHON)" scripts/test.sh --tools-only
reproduce-smoke:
	@RUN_TAG="$(RUN_TAG)" PYTHON="$(PYTHON)" scripts/reproduce_smoke.sh
trace-smoke:
	@RUN_TAG="$(RUN_TAG)" PYTHON="$(PYTHON)" experiments/11_trace_smoke/run.sh
paper-plan:
	@$(PYTHON) experiments/reproduce.py --plan --suite all
reproduce-paper:
	@RUN_TAG="$(RUN_TAG)" MAX_PAR="$(MAX_PAR)" SUITE="$(SUITE)" PYTHON="$(PYTHON)" scripts/reproduce_paper.sh
setup-mqsim:
	@JOBS="$(JOBS)" scripts/setup_mqsim.sh
figures:
	@$(PYTHON) artifact/figures/gen_media_saturation.py --output-dir artifact_runs/figures
	@$(PYTHON) artifact/figures/gen_case_studies.py --output-dir artifact_runs/figures
	@$(PYTHON) experiments/25_t02_t03_comparison/figures/gen_fig_comparison.py --data-dir artifact/reference --output-dir artifact_runs/figures
container:
	@docker build -t hbf-sim:dev .
# Local manuscript workflow, available only when its private builder exists.
ifneq ($(wildcard scripts/build_paper.sh),)
.PHONY: paper
paper:
	@bash scripts/build_paper.sh
endif
.PHONY: prepare-qwen
prepare-qwen:
	@$(PYTHON) scripts/unpack_qwen.py
