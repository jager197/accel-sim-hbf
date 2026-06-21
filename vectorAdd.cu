#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

__global__ void vectorAdd(float *a, float *b, float *c, int n) {
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx < n) {
        c[idx] = a[idx] + b[idx];
    }
}

int main() {
    int n = 1024;
    size_t bytes = n * sizeof(float);

    float *h_a = (float*)malloc(bytes);
    float *h_b = (float*)malloc(bytes);
    float *h_c = (float*)malloc(bytes);

    for (int i = 0; i < n; i++) {
        h_a[i] = sinf(i) * sinf(i);
        h_b[i] = cosf(i) * cosf(i);
    }

    float *d_a, *d_b, *d_c;
    cudaMalloc(&d_a, bytes);
    cudaMalloc(&d_b, bytes);
    cudaMalloc(&d_c, bytes);
    cudaMemcpy(d_a, h_a, bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, h_b, bytes, cudaMemcpyHostToDevice);

    int threads = 256;
    int blocks = (n + threads - 1) / threads;
    vectorAdd<<<blocks, threads>>>(d_a, d_b, d_c, n);

    cudaMemcpy(h_c, d_c, bytes, cudaMemcpyDeviceToHost);

    int errors = 0;
    for (int i = 0; i < n; i++) {
        if (fabs(h_c[i] - 1.0f) > 1e-5) errors++;
    }
    printf("Test %s: %d errors out of %d\n", errors ? "FAILED" : "PASSED", errors, n);

    cudaFree(d_a); cudaFree(d_b); cudaFree(d_c);
    free(h_a); free(h_b); free(h_c);
    return errors ? 1 : 0;
}
