// KV SWA — KV-cache sliding-window attention workload for the HBF experiment.
//
// Models the attention read phase of autoregressive LLM inference: each token
// reads the KV entries of the previous `window` tokens. The KV cache is split
// into two regions by a DRAM capacity threshold D:
//
//   entry i <  D  -> DRAM region   (dram_base + i*128, low addresses)
//   entry i >= D  -> HBF region    (hbf_base  + (i-D)*128, 256 GB+)
//
// With `dram_entries == n_tokens` the workload degenerates to "unlimited
// DRAM" (all entries resident in DRAM). The experiment cases differ ONLY in
// the simulator config + this dram_entries parameter; the kernel and access
// pattern are identical, so performance differences are attributable to the
// memory tier.
//
// One warp simulates the whole autoregressive sequence sequentially (token t
// depends on token t-1), which matches real LLM inference and keeps the
// number of in-flight memory requests small. Each lane reads one 4B word of
// a 128B KV entry -> one coalesced 128B request per window step.
// Only reads are modeled (attention phase); KV writes are not part of this
// experiment.

#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

// Address of KV entry i in bytes. DRAM entries via the cudaMalloc'd base,
// HBF entries via the raw 256GB base (generic pointer, ld.f32 path).
#define KV_ADDR(i)                                                          \
  (((unsigned long long)(i) < (unsigned long long)dram_entries)             \
       ? ((unsigned long long)(size_t)(dram_base) +                         \
          (unsigned long long)(i) * 128ULL)                                 \
       : (hbf_base + ((unsigned long long)(i) -                            \
                      (unsigned long long)dram_entries) * 128ULL))

__global__ void kv_swa_kernel(const float *dram_base, unsigned long long hbf_base,
                              int n_tokens, int dram_entries, int window,
                              float *out) {
  int lane = threadIdx.x;

  // Sequential autoregressive pass: token t reads the KV of the previous
  // `window` tokens (attention), one token at a time.
  float acc = 0.0f;
  for (int t = 0; t < n_tokens; t++) {
    for (int w = 1; w <= window; w++) {
      int i = t - w;
      if (i >= 0) {
        acc += *(const float *)(KV_ADDR(i) + (unsigned long long)lane * 4ULL);
      }
    }
    // Write result to DRAM output (prevents dead-code elimination, verifies
    // the reads actually happened).
    out[(unsigned long long)t * 32ULL + lane] = acc;
  }
}

int main(int argc, char **argv) {
  // Args: <n_tokens> <dram_entries> <window> [hbf_base]
  int n_tokens = argc > 1 ? atoi(argv[1]) : 16;
  int dram_entries = argc > 2 ? atoi(argv[2]) : 8;
  int window = argc > 3 ? atoi(argv[3]) : 4;
  unsigned long long hbf_base =
      argc > 4 ? strtoull(argv[4], NULL, 0) : 274877906944ULL;

  // DRAM-resident KV region (first dram_entries entries, 128 B each)
  size_t dram_bytes = (size_t)dram_entries * 128ULL;
  float *h_dram = (float *)calloc(dram_bytes / 4 + 1, sizeof(float));
  float *dram_base = NULL;
  cudaMalloc((void **)&dram_base, dram_bytes);
  cudaMemcpy(dram_base, h_dram, dram_bytes, cudaMemcpyHostToDevice);

  // Output accumulator (DRAM)
  size_t out_bytes = (size_t)n_tokens * 32ULL * sizeof(float);
  float *h_out = (float *)calloc(n_tokens * 32ULL + 1, sizeof(float));
  float *d_out = NULL;
  cudaMalloc((void **)&d_out, out_bytes);

  kv_swa_kernel<<<1, 32>>>(dram_base, hbf_base, n_tokens, dram_entries,
                           window, d_out);
  cudaDeviceSynchronize();

  cudaMemcpy(h_out, d_out, out_bytes, cudaMemcpyDeviceToHost);
  printf("[KV-SWA] n_tokens=%d dram_entries=%d window=%d hbf_base=%llu out[0]=%f\n",
         n_tokens, dram_entries, window, hbf_base, h_out[0]);

  cudaFree(dram_base);
  cudaFree(d_out);
  free(h_dram);
  free(h_out);
  return 0;
}
