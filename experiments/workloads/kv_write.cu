// kv_write — LLM 推理 KV cache 追加写负载
//
// 场景: 每个 decode step 把新 token 的 K/V 追加到 KV cache。
// 对应 OCP HBF 规格 Ch13 "KV Cache data write/read guidelines"。
//
// 访问模式: n_entries 个 warp 各写一条 128 B KV 条目（32 lane × 4 B），
// 条目地址连续 —— 同一 4 KB page 的 32 条写经 HBF 写缓冲合并为一次
// NAND 页编程；新块首次编程前触发一次块擦除（erase-before-write）。
//
// 运行: kv_write <n_entries> <mode> [hbf_base]
//   n_entries KV 条目数（默认 256）
//   mode      0 = DRAM（写后读回校验）  1 = HBF（时序建模）
//   hbf_base  HBF 起始地址（默认 256 GB）

#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

__global__ void kw_kernel(unsigned long long base, int n_entries) {
    int tid = blockDim.x * blockIdx.x + threadIdx.x;
    int lane = tid & 31;
    int wid = tid >> 5;
    if (wid < n_entries) {
        float *p = (float *)(base + (unsigned long long)wid * 128ULL +
                             (unsigned long long)lane * 4ULL);
        *p = (float)(wid * 32 + lane);   // 追加写：值 = 全局序号
    }
}

// 读回校验（仅 DRAM 模式使用；HBF 模式不做数据校验）
__global__ void kw_readback(unsigned long long base, float *out, int n_entries) {
    int tid = blockDim.x * blockIdx.x + threadIdx.x;
    int lane = tid & 31;
    int wid = tid >> 5;
    if (wid < n_entries) {
        float *p = (float *)(base + (unsigned long long)wid * 128ULL +
                             (unsigned long long)lane * 4ULL);
        out[(unsigned long long)wid * 32ULL + lane] = *p;
    }
}

int main(int argc, char **argv) {
    int n_entries = argc > 1 ? atoi(argv[1]) : 256;
    int mode = argc > 2 ? atoi(argv[2]) : 0;
    unsigned long long hbf_base =
        argc > 3 ? strtoull(argv[3], NULL, 0) : 274877906944ULL;

    int threads = 256;
    int blocks = (n_entries * 32 + threads - 1) / threads;

    float *h_data = (float *)calloc((size_t)n_entries * 32ULL, sizeof(float));
    float *d_data = NULL;
    cudaMalloc((void **)&d_data, (size_t)n_entries * 128ULL);

    unsigned long long base = (mode == 0)
        ? (unsigned long long)(size_t)d_data   // DRAM: 设备内存地址
        : hbf_base;                             // HBF: 256 GB 段

    kw_kernel<<<blocks, threads>>>(base, n_entries);
    cudaDeviceSynchronize();

    int errors = -1;
    if (mode == 0) {
        kw_readback<<<blocks, threads>>>(base, d_data, n_entries);
        cudaDeviceSynchronize();
        cudaMemcpy(h_data, d_data, (size_t)n_entries * 128ULL,
                   cudaMemcpyDeviceToHost);
        errors = 0;
        for (int i = 0; i < n_entries * 32; i++)
            if (fabs(h_data[i] - (float)i) > 0.1f) errors++;
    }
    printf("[KV-WRITE] n_entries=%d mode=%d errors=%d\n",
           n_entries, mode, errors);

    cudaFree(d_data);
    free(h_data);
    return mode == 0 && errors ? 1 : 0;
}
