// weight_load — LLM 权重 / 预填充顺序流式读取负载
//
// 场景: 推理初始化或 prefill 阶段把模型权重从近存储层顺序读入。
// 对应 OCP HBF 规格 Ch13 "AI Parameter Store & Loading"。
//
// 访问模式: W 个 warp 各自按 grid-stride 顺序读取连续的 128 B 条目
// （32 lane × 4 B = 128 B / 请求）。同一时刻活跃的 warp 覆盖一段连续
// 地址，MSHR 把同一 4 KB page 的 32 个请求合并为一次 NAND 页读 ——
// 顺序流的 HBF 聚合带宽主要由 (分区数 × max_active × 4 KB / tR) 决定。
//
// 性能要点（对模拟器与真实 GPU 都成立）:
//   1. 4 路展开 + 4 个独立累加器: 每路的 load 目标寄存器互不相同，
//      避免寄存器 WAR 让同一 warp 的读串行化 —— 每 warp 最多 4 个读在飞。
//   2. 1024 warps（默认）: 足够的在飞请求喂满 HBF 子阵列槽位
//      （8 分区 × 64 并发 = 512 个页读槽位）。
//      注意不要开更多 —— GPGPU-Sim 的 scoreboard 每周期对每个 warp
//      构造/析构 std::set，warp 过多会拖垮仿真墙钟时间。
//
// 运行: weight_load <size_mb> <mode> [hbf_base] [blocks]
//   size_mb  读取总量（MB），默认 1
//   mode     0 = DRAM（数据可校验）  1 = HBF（时序建模）
//   hbf_base HBF 起始地址（默认 256 GB）
//   blocks   grid 的 block 数（默认 128 = 1024 warps）

#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ENTRY_BYTES 128ULL

__device__ __forceinline__ const float *wl_ptr(const float *dram_base,
                                               unsigned long long hbf_base,
                                               unsigned long long e, int mode,
                                               int lane) {
    if (mode == 0)
        return (const float *)((const char *)dram_base + e * ENTRY_BYTES +
                               (unsigned long long)lane * 4ULL);
    return (const float *)(hbf_base + e * ENTRY_BYTES +
                           (unsigned long long)lane * 4ULL);
}

__global__ void wl_kernel(const float *dram_base, unsigned long long hbf_base,
                          unsigned long long n_entries, int mode, float *out) {
    int tid = blockDim.x * blockIdx.x + threadIdx.x;
    int lane = tid & 31;
    unsigned long long wid = (unsigned long long)tid >> 5;
    unsigned long long n_warps =
        ((unsigned long long)gridDim.x * (unsigned long long)blockDim.x) >> 5;

    // 8 路全展开: 每轮先发全部 8 个 load（8 个独立目标寄存器 → 8 个读在飞），
    // 再统一累加。GPGPU-Sim 是顺序发射：如果 load 后面紧跟依赖其数据的 add，
    // 后续 load 会被挡在该 add 后面 → 每 warp 在飞读只有 ~2 个；把 add 全部
    // 挪到 load 之后，每 warp 可保 8 个读在飞（1024 warps × 8 ≈ 8192 请求）。
    float a0 = 0.0f, a1 = 0.0f, a2 = 0.0f, a3 = 0.0f;
    float a4 = 0.0f, a5 = 0.0f, a6 = 0.0f, a7 = 0.0f;
    unsigned long long e = wid;
    for (; e + 7ULL * n_warps < n_entries; e += 8ULL * n_warps) {
        float l0 = *wl_ptr(dram_base, hbf_base, e, mode, lane);
        float l1 = *wl_ptr(dram_base, hbf_base, e + n_warps, mode, lane);
        float l2 = *wl_ptr(dram_base, hbf_base, e + 2ULL * n_warps, mode, lane);
        float l3 = *wl_ptr(dram_base, hbf_base, e + 3ULL * n_warps, mode, lane);
        float l4 = *wl_ptr(dram_base, hbf_base, e + 4ULL * n_warps, mode, lane);
        float l5 = *wl_ptr(dram_base, hbf_base, e + 5ULL * n_warps, mode, lane);
        float l6 = *wl_ptr(dram_base, hbf_base, e + 6ULL * n_warps, mode, lane);
        float l7 = *wl_ptr(dram_base, hbf_base, e + 7ULL * n_warps, mode, lane);
        a0 += l0; a1 += l1; a2 += l2; a3 += l3;
        a4 += l4; a5 += l5; a6 += l6; a7 += l7;
    }
    // 收尾
    for (; e < n_entries; e += n_warps)
        a0 += *wl_ptr(dram_base, hbf_base, e, mode, lane);

    // lane 0 的累加结果写入 DRAM（防死代码消除 + 宿主校验锚点）
    if (lane == 0)
        out[wid] = ((a0 + a1) + (a2 + a3)) + ((a4 + a5) + (a6 + a7));
}

int main(int argc, char **argv) {
    int size_mb = argc > 1 ? atoi(argv[1]) : 1;
    int mode = argc > 2 ? atoi(argv[2]) : 0;
    unsigned long long hbf_base =
        argc > 3 ? strtoull(argv[3], NULL, 0) : 274877906944ULL;
    int blocks = argc > 4 ? atoi(argv[4]) : 128;

    unsigned long long total_bytes = (unsigned long long)size_mb * 1024ULL * 1024ULL;
    unsigned long long n_entries = total_bytes / ENTRY_BYTES;

    // 256 threads/block；warp 数 = blocks×8（默认 128 blocks = 1024 warps）
    const int threads = 256;
    unsigned long long n_warps = (unsigned long long)blocks * threads / 32ULL;

    // DRAM 数据区（mode 0 时用已知 pattern 初始化以便校验）
    float *h_dram = (float *)malloc(total_bytes);
    for (unsigned long long i = 0; i < total_bytes / 4ULL; i++)
        h_dram[i] = (float)(i & 0xFF);
    float *dram_base = NULL;
    cudaMalloc((void **)&dram_base, total_bytes);
    cudaMemcpy(dram_base, h_dram, total_bytes, cudaMemcpyHostToDevice);

    // 输出累加器（每 warp 一个 float）
    float *h_out = (float *)calloc(n_warps, sizeof(float));
    float *d_out = NULL;
    cudaMalloc((void **)&d_out, n_warps * sizeof(float));

    wl_kernel<<<blocks, threads>>>(dram_base, hbf_base, n_entries, mode, d_out);
    cudaDeviceSynchronize();

    cudaMemcpy(h_out, d_out, n_warps * sizeof(float), cudaMemcpyDeviceToHost);

    // DRAM 模式校验：warp w 的期望值 = Σ h_dram[e*32] （lane 0 的字），
    // e 遍历 w 步长 n_warps 的条目编号
    int errors = 0;
    if (mode == 0) {
        for (unsigned long long w = 0; w < n_warps; w++) {
            float expect = 0.0f;
            for (unsigned long long e = w; e < n_entries; e += n_warps)
                expect += (float)((e * 32ULL) & 0xFF);
            if (fabs(h_out[w] - expect) > 0.5f) errors++;
        }
    }
    printf("[WEIGHT-LOAD] size_mb=%d mode=%d n_entries=%llu n_warps=%llu "
           "errors=%d out[0]=%f\n",
           size_mb, mode, n_entries, n_warps, errors, h_out[0]);

    cudaFree(dram_base);
    cudaFree(d_out);
    free(h_dram);
    free(h_out);
    return mode == 0 && errors ? 1 : 0;
}
