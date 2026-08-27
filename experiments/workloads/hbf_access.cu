// hbf_access — HBF 冒烟测试负载
//
// 三部分，覆盖 HBF 模拟器的关键路径:
//   1. [DRAM Test]  标准 DRAM 读写 + 数据校验 —— 功能模型完好性
//   2. [HBF-READ]   HBF 地址段读取（未初始化页，时序建模）
//   3. [HBF-RW]     HBF 地址段写入 → 读回 → 数据比对（写缓冲→擦除→编程→
//                   读回全链路，校验功能数据在 HBF 路径上不丢失）
//
// 运行: hbf_access [hbf_base] [do_rw]
//   hbf_base  HBF 起始地址（默认 256 GB = 274877906944）
//   do_rw     非 0 时运行 Part 3（写→读回，含 2M 周期块擦除，耗时长）
//
// HBF 段访问通过裸地址解引用（ld/st 通用指针路径）触发 is_hbf_addr() 路由。

#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// ---- Part 1: DRAM 正确性（照搬 vector_add 思路，就地 2x 缩放） ----
__global__ void dram_scale(float *dram_data, int n) {
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx < n) {
        float val = dram_data[idx];
        dram_data[idx] = val * 2.0f;
    }
}

// ---- Part 2: HBF 地址段读取 ----
__global__ void hbf_read(unsigned long long hbf_base, float *dram_out, int n) {
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx < n) {
        float *hbf_ptr = (float *)(hbf_base + (unsigned long long)idx * 4ULL);
        float val = hbf_ptr[0];           // 路由到 HBF 的读
        dram_out[idx] = val + 1.0f;       // 结果写回 DRAM
    }
}

// ---- Part 3: HBF 地址段写入 ----
__global__ void hbf_write(unsigned long long hbf_base, int n) {
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx < n) {
        float *hbf_ptr = (float *)(hbf_base + (unsigned long long)idx * 4ULL);
        *hbf_ptr = (float)(idx + 1);      // 路由到 HBF 的写（写缓冲/擦除/编程路径）
    }
}

// ---- Part 3b: HBF 地址段读回 ----
__global__ void hbf_readback(unsigned long long hbf_base, float *dram_out, int n) {
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx < n) {
        float *hbf_ptr = (float *)(hbf_base + (unsigned long long)idx * 4ULL);
        dram_out[idx] = hbf_ptr[0];
    }
}

int main(int argc, char **argv) {
    int n = 1024;  // 4 KB = 一个 NAND page
    unsigned long long hbf_base =
        argc > 1 ? strtoull(argv[1], NULL, 0) : 274877906944ULL;
    int do_rw = argc > 2 ? atoi(argv[2]) : 0;
    size_t bytes = (size_t)n * sizeof(float);

    // ================= Test 1: DRAM correctness =================
    float *h_data = (float *)malloc(bytes);
    for (int i = 0; i < n; i++) h_data[i] = (float)i;

    float *d_data;
    cudaMalloc(&d_data, bytes);
    cudaMemcpy(d_data, h_data, bytes, cudaMemcpyHostToDevice);

    dram_scale<<<4, 256>>>(d_data, n);
    cudaDeviceSynchronize();

    cudaMemcpy(h_data, d_data, bytes, cudaMemcpyDeviceToHost);
    int errors = 0;
    for (int i = 0; i < n; i++)
        if (fabs(h_data[i] - (float)(i * 2)) > 0.1f) errors++;
    printf("[DRAM Test] %s: %d errors out of %d\n",
           errors ? "FAILED" : "PASSED", errors, n);
    if (errors) return 1;

    // ================= Test 2: HBF region read =================
    float *h_out = (float *)calloc(n, sizeof(float));
    float *d_out;
    cudaMalloc(&d_out, bytes);
    cudaMemcpy(d_out, h_out, bytes, cudaMemcpyHostToDevice);

    hbf_read<<<4, 256>>>(hbf_base, d_out, n);
    cudaDeviceSynchronize();
    printf("[HBF-READ] completed (HBF 未初始化区, 不校验具体值)\n");

    // ================= Test 3: HBF write → readback =================
    if (do_rw) {
      hbf_write<<<4, 256>>>(hbf_base, n);
      cudaDeviceSynchronize();
      hbf_readback<<<4, 256>>>(hbf_base, d_out, n);
      cudaDeviceSynchronize();

      cudaMemcpy(h_out, d_out, bytes, cudaMemcpyDeviceToHost);
      int rw_errors = 0;
      for (int i = 0; i < n; i++)
          if (fabs(h_out[i] - (float)(i + 1)) > 0.1f) rw_errors++;
      printf("[HBF-RW] %s: %d errors out of %d (write->readback round-trip "
             "through HBF erase/program/read path)\n",
             rw_errors ? "FAILED" : "PASSED", rw_errors, n);
      if (rw_errors) return 1;
    }

    cudaFree(d_data);
    cudaFree(d_out);
    free(h_data);
    free(h_out);
    printf("hbf_access smoke test complete.\n");
    return 0;
}
