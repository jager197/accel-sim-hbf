// HBF traffic test: data over L2 capacity to force misses for trace-driven replay.
#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define N (3 * 1024 * 1024)
#define THREADS 256
__global__ void stream_read(float *in, float *out, int n) {
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx < n) out[idx] = in[idx] * 2.0f;
}
int main() {
    size_t bytes = N * sizeof(float);
    float *h_in = (float*)malloc(bytes), *h_out = (float*)malloc(bytes);
    for (int i = 0; i < N; i++) h_in[i] = (float)(i & 0xFF);
    float *d_in, *d_out;
    cudaMalloc(&d_in, bytes); cudaMalloc(&d_out, bytes);
    cudaMemcpy(d_in, h_in, bytes, cudaMemcpyHostToDevice);
    int blocks = (N + THREADS - 1) / THREADS;
    stream_read<<<blocks, THREADS>>>(d_in, d_out, N);
    cudaDeviceSynchronize();
    cudaMemcpy(h_out, d_out, bytes, cudaMemcpyDeviceToHost);
    int errors = 0;
    for (int i = 0; i < N; i++)
        if (fabs(h_out[i] - h_in[i] * 2.0f) > 0.1f) errors++;
    printf("Test %s: %d/%d errors\n", errors?"FAILED":"PASSED", errors, N);
    cudaFree(d_in); cudaFree(d_out);
    free(h_in); free(h_out);
    return errors ? 1 : 0;
}
