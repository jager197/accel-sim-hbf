#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>

// Independent lanes touch different pages: one 32 B sector request triggers
// one 4 KiB array read. Output is a liveness sink, not a data-integrity test.
__global__ void page_supply(unsigned long long base, unsigned int *out,
                            int pages, int producers) {
  const int tid = blockIdx.x * blockDim.x + threadIdx.x;
  if (tid >= producers) return;
  unsigned int sum = 0;
  for (int page = tid; page < pages; page += producers) {
    volatile unsigned int *source =
        (volatile unsigned int *)(base + (unsigned long long)page * 4096ULL);
    sum ^= source[0];
  }
  out[tid] = sum;
}

int main(int argc, char **argv) {
  const unsigned long long base = argc > 1 ? strtoull(argv[1], NULL, 0) : 274877906944ULL;
  const int pages = argc > 2 ? atoi(argv[2]) : 1024;
  const int producers = argc > 3 ? atoi(argv[3]) : pages;
  if (pages <= 0 || pages > 65536 || producers <= 0 || producers > pages ||
      producers % 32 != 0 || pages % producers != 0) return 2;
  unsigned int *out = NULL;
  if (cudaMalloc(&out, producers * sizeof(*out)) != cudaSuccess) return 1;
  page_supply<<<(producers + 127) / 128, 128>>>(base, out, pages, producers);
  cudaError_t status = cudaDeviceSynchronize();
  if (status != cudaSuccess) {
    fprintf(stderr, "page supply failed: %s\n", cudaGetErrorString(status));
    cudaFree(out);
    return 1;
  }
  cudaFree(out);
  printf("[PAGE-SUPPLY] completed: %d pages, %d producers\n", pages, producers);
  return 0;
}
