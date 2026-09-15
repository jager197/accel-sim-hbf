// Page-strided global reads used to validate logical-cube channel ownership.

#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

__global__ void channel_probe(unsigned long long hbf_base, unsigned int *out,
                              int pages, int page_stride) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid < pages) {
        unsigned long long address =
            hbf_base + (unsigned long long)tid * (unsigned long long)page_stride;
        volatile unsigned int *ptr = (volatile unsigned int *)address;
        out[tid] = ptr[0];
    }
}

int main(int argc, char **argv) {
    unsigned long long hbf_base =
        argc > 1 ? strtoull(argv[1], NULL, 0) : 274877906944ULL;
    int pages = argc > 2 ? atoi(argv[2]) : 64;
    int page_stride = argc > 3 ? atoi(argv[3]) : 4096;
    if (pages <= 0 || pages > 4096 || page_stride < 4096 ||
        (page_stride % 4096) != 0) {
        fprintf(stderr, "pages must be in [1,4096] and page_stride >= 4096\n");
        return 2;
    }
    unsigned int *out = NULL;
    cudaError_t status = cudaMalloc(&out, (size_t)pages * sizeof(*out));
    if (status != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed: %s\n", cudaGetErrorString(status));
        return 1;
    }
    channel_probe<<<(pages + 127) / 128, 128>>>(hbf_base, out, pages,
                                                 page_stride);
    status = cudaDeviceSynchronize();
    if (status != cudaSuccess) {
        fprintf(stderr, "channel probe failed: %s\n", cudaGetErrorString(status));
        cudaFree(out);
        return 1;
    }
    printf("[CHANNEL-PROBE] completed: %d page-strided global loads\n", pages);
    cudaFree(out);
    return 0;
}
