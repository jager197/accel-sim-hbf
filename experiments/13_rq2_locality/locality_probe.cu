#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>

__global__ void locality_probe(unsigned long long base, unsigned int *out,
                               int pages, int entries_per_page) {
  int tid = blockIdx.x * blockDim.x + threadIdx.x;
  int lane = tid & 31;
  int warp = tid >> 5;
  int warps = pages * entries_per_page;
  if (warp >= warps) return;
  int page = warp / entries_per_page;
  int entry = warp % entries_per_page;
  unsigned long long address =
      base + (unsigned long long)page * 4096ULL +
      (unsigned long long)entry * 128ULL + (unsigned long long)lane * 4ULL;
  volatile unsigned int *source = (volatile unsigned int *)address;
  unsigned int value = source[0];
  if (lane == 0) out[warp] = value;
}

int main(int argc, char **argv) {
  int pages = argc > 1 ? atoi(argv[1]) : 16;
  int entries = argc > 2 ? atoi(argv[2]) : 8;
  unsigned long long base =
      argc > 3 ? strtoull(argv[3], NULL, 0) : 274877906944ULL;
  if (pages <= 0 || pages > 4096 || entries <= 0 || entries > 32) {
    fprintf(stderr, "pages must be in [1,4096], entries_per_page in [1,32]\n");
    return 2;
  }
  int warps = pages * entries;
  unsigned int *out = NULL;
  cudaError_t status = cudaMalloc(&out, (size_t)warps * sizeof(*out));
  if (status != cudaSuccess) {
    fprintf(stderr, "cudaMalloc failed: %s\n", cudaGetErrorString(status));
    return 1;
  }
  int threads = warps * 32;
  locality_probe<<<(threads + 255) / 256, 256>>>(base, out, pages, entries);
  status = cudaDeviceSynchronize();
  if (status != cudaSuccess) {
    fprintf(stderr, "locality probe failed: %s\n", cudaGetErrorString(status));
    cudaFree(out);
    return 1;
  }
  printf("[LOCALITY-PROBE] completed: %d pages x %d entries\n", pages,
         entries);
  cudaFree(out);
  return 0;
}
