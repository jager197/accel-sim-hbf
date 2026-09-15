#include <cuda_runtime.h>

#include <cstdio>
#include <cstdlib>

__device__ unsigned long long selected_page(unsigned warp,
                                            unsigned long long total_pages) {
  if (warp == 0) return 0;
  if (warp == 1) return total_pages / 4 + 1;
  if (warp == 2) return total_pages / 2 + 2;
  if (warp == 3) return (3 * total_pages) / 4 + 3;
  return total_pages - 1;
}

__global__ void sparse_capacity_read(unsigned long long base,
                                     unsigned long long span,
                                     unsigned int *output) {
  unsigned tid = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned lane = tid & 31u;
  unsigned warp = tid >> 5;
  if (warp >= 5) return;
  unsigned long long page = selected_page(warp, span / 4096ull);
  unsigned long long address = base + page * 4096ull + lane * sizeof(unsigned);
  volatile unsigned *source = reinterpret_cast<volatile unsigned *>(address);
  unsigned value = source[0];
  if (lane == 0) output[warp] = value;
}

int main(int argc, char **argv) {
  unsigned long long base =
      argc > 1 ? std::strtoull(argv[1], nullptr, 0) : 274877906944ull;
  unsigned long long span =
      argc > 2 ? std::strtoull(argv[2], nullptr, 0) : 549755813888ull;
  if (span < 4096 || span % 4096 != 0) {
    std::fprintf(stderr, "span must contain whole 4 KiB pages\n");
    return 2;
  }

  unsigned int *output = nullptr;
  if (cudaMalloc(&output, 5 * sizeof(*output)) != cudaSuccess) {
    std::fprintf(stderr, "cudaMalloc failed\n");
    return 1;
  }
  sparse_capacity_read<<<1, 160>>>(base, span, output);
  cudaError_t status = cudaDeviceSynchronize();
  cudaFree(output);
  if (status != cudaSuccess) {
    std::fprintf(stderr, "sparse capacity probe failed: %s\n",
                 cudaGetErrorString(status));
    return 1;
  }
  std::printf("[SPARSE-CAPACITY] completed: 5 pages across %llu bytes\n", span);
  return 0;
}
