FROM nvidia/cuda:12.0.1-devel-ubuntu22.04

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
       bash bison build-essential ca-certificates cmake flex git \
       freeglut3-dev libboost-all-dev libgl1-mesa-dev libglu1-mesa-dev \
       python3 python3-pip python3-venv xutils-dev zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

ENV CUDA_INSTALL_PATH=/usr/local/cuda \
    VIRTUAL_ENV=/opt/hbf-venv
RUN python3 -m venv "$VIRTUAL_ENV"
ENV PATH="$VIRTUAL_ENV/bin:/usr/local/cuda/bin:$PATH"

COPY requirements.txt /tmp/hbf-requirements.txt
RUN python -m pip install --no-cache-dir --upgrade pip \
    && python -m pip install --no-cache-dir -r /tmp/hbf-requirements.txt

WORKDIR /workspace/hbf-sim
CMD ["bash"]
