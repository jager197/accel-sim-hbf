// hbf_mixed — E4: mixed foreground-read / background-write workload
//
// Foreground: n_read warps repeatedly read their own 128B entry (windowed
// reuse — page-buffer/page-cache friendly). Background: n_write warps
// append-write 128B entries into a separate region. Both flows share the
// same host channels/sub-arrays (interleave mapping), so writes contend
// with reads at the sub-array and write-buffer level.
//
// The experiment compares HBF schedulers (FCFS / read-priority /
// write-drain) under increasing write pressure.
//
// Run: hbf_mixed <n_read> <n_write> <window> <hbf_base>
#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

__global__ void mixed_kernel(unsigned long long base, int n_read,
                             int n_write, int window, int tmax) {
    int tid = blockDim.x * blockIdx.x + threadIdx.x;
    int lane = tid & 31;
    int wid = tid >> 5;
    if (wid < n_read) {
        // Foreground: T iterations over DISTINCT entries (no reuse), so
        // every load misses the page cache/page buffers and must queue on
        // the NAND array -- contention with writes is real.
        float acc = 0.0f;
        for (int t = 0; t < tmax; t++) {
            int e = wid + t * n_read;
            float *p = (float *)(base + (unsigned long long)e * 128ULL +
                                 (unsigned long long)lane * 4ULL);
            acc += p[0];
        }
        if (acc == 12345.678f) asm volatile("");  // keep the loads
    } else if (wid < n_read + n_write) {
        // Background: one append-write of a 128B entry in a separate region.
        int w = wid - n_read;
        float *p = (float *)(base + (1ULL << 20) +
                             (unsigned long long)w * 128ULL +
                             (unsigned long long)lane * 4ULL);
        *p = (float)w;
    }
}

int main(int argc, char **argv) {
    int n_read = argc > 1 ? atoi(argv[1]) : 64;
    int n_write = argc > 2 ? atoi(argv[2]) : 16;
    int tmax = argc > 3 ? atoi(argv[3]) : 256;
    unsigned long long hbf_base =
        argc > 4 ? strtoull(argv[4], NULL, 0) : 274877906944ULL;

    int total_warps = n_read + n_write;
    mixed_kernel<<<(total_warps + 7) / 8, 256>>>(hbf_base, n_read, n_write,
                                                 tmax, tmax);
    cudaDeviceSynchronize();
    printf("[HBF-MIXED] completed: %d read warps x %d iters, %d write warps\n",
           n_read, tmax, n_write);
    return 0;
}
