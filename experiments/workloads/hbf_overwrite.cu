// hbf_overwrite — E3: two-pass overwrite workload
//
// Pass 1: write entries 0..n_entries-1 (fills blocks with valid pages).
// Pass 2: overwrite entries 0..n_entries/2-1 (invalidates half the pages,
// leaving the other half valid in the same blocks).
//
// Under SSD semantics (media_mode=1), blocks containing a mix of valid and
// invalid pages force GREEDY GC to RELOCATE the valid pages (copies +
// stall cycles). Under HBF semantics (media_mode=0, OCP §11.4) the device
// never moves valid data; only fully-invalidated blocks return to the free
// pool, so no relocation traffic exists. The identical request stream in
// both modes quantifies the error of reusing SSD abstractions for HBF.
//
// Run: hbf_overwrite <n_entries> <hbf_base>
//      (use -gpgpu_hbf_pages_per_block 4 so blocks fill quickly)
#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

__global__ void ow_kernel(unsigned long long base, int n_entries) {
    int tid = blockDim.x * blockIdx.x + threadIdx.x;
    int lane = tid & 31;
    int wid = tid >> 5;
    if (wid < n_entries) {
        float *p = (float *)(base + (unsigned long long)wid * 128ULL +
                             (unsigned long long)lane * 4ULL);
        *p = (float)(wid * 32 + lane);           // pass 1
    }
    if (wid < n_entries / 2) {
        float *p = (float *)(base + (unsigned long long)wid * 128ULL +
                             (unsigned long long)lane * 4ULL);
        *p = (float)(wid * 32 + lane + 1);       // pass 2 (overwrite)
    }
}

int main(int argc, char **argv) {
    int n_entries = argc > 1 ? atoi(argv[1]) : 2048;
    unsigned long long hbf_base =
        argc > 2 ? strtoull(argv[2], NULL, 0) : 274877906944ULL;

    // 256 threads/block = 8 warps/block; need n_entries warps total.
    ow_kernel<<<(n_entries + 7) / 8, 256>>>(hbf_base, n_entries);
    cudaDeviceSynchronize();
    printf("[HBF-OVERWRITE] completed: %d entries, two-pass\n", n_entries);
    return 0;
}
