// Explicit global-to-shared staging workload.
//
// The first load uses an HBF global address.  The store and the following load
// use CUDA shared memory explicitly.  There is no device-side DMA in this
// kernel; the shared-memory traffic is generated only by these instructions.

#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

__global__ void shared_stage(unsigned long long hbf_base, float *out, int n) {
    __shared__ float tile[128];
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int lane = threadIdx.x;
    if (tid < n) {
        volatile float *hbf_ptr =
            (volatile float *)(hbf_base + (unsigned long long)tid * 4ULL);
        tile[lane] = hbf_ptr[0];
    }
    __syncthreads();
    if (tid < n) {
        out[tid] = tile[(lane + 1) & 127];
    }
}

int main(int argc, char **argv) {
    unsigned long long hbf_base =
        argc > 1 ? strtoull(argv[1], NULL, 0) : 274877906944ULL;
    int n = argc > 2 ? atoi(argv[2]) : 256;
    if (n <= 0 || n > 4096) {
        fprintf(stderr, "n must be in [1,4096]\n");
        return 2;
    }
    size_t bytes = (size_t)n * sizeof(float);
    float *out = NULL;
    cudaError_t status = cudaMalloc(&out, bytes);
    if (status != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed: %s\n", cudaGetErrorString(status));
        return 1;
    }
    shared_stage<<<(n + 127) / 128, 128>>>(hbf_base, out, n);
    status = cudaDeviceSynchronize();
    if (status != cudaSuccess) {
        fprintf(stderr, "shared-stage failed: %s\n", cudaGetErrorString(status));
        cudaFree(out);
        return 1;
    }
    printf("[SHARED-STAGING] completed: %d HBF global loads followed by explicit shared staging\n", n);
    cudaFree(out);
    return 0;
}
