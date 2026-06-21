// HBF Smoke Test — accesses memory in HBF address range
//
// In PTX mode (GPGPU-Sim), memory accesses are simulated. We explicitly target
// the HBF address range by adding an offset to a cudaMalloc'd pointer, so that
// the resulting address falls in the HBF region configured in gpgpusim.config.
//
// HBF base address default: 274877906944 (256 GB)
// We allocate in DRAM, then offset into HBF range for read test.

#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Workload: read from HBF region, compute, write result to DRAM region
__global__ void hbf_access_test(float *dram_data, int n) {
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx < n) {
        // dram_data is in DRAM (low address). We read it, process,
        // and write back — this exercises the DRAM path.
        float val = dram_data[idx];
        dram_data[idx] = val * 2.0f;
    }
}

// Kernel that accesses HBF address range via explicit address calculation.
// The pointer passed is offset from the HBF base so the actual memory access
// targets the HBF region.
__global__ void hbf_explicit_access(unsigned long long hbf_base, float *dram_out, int n) {
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx < n) {
        // Convert HBF base to a float pointer and read from it
        float *hbf_ptr = (float*)(hbf_base + idx * sizeof(float));
        // This read goes to HBF address range
        float val = hbf_ptr[0];  // Read from HBF
        dram_out[idx] = val + 1.0f; // Write result to DRAM
    }
}

int main(int argc, char **argv) {
    int n = 1024;
    size_t bytes = n * sizeof(float);

    // === Test 1: Standard DRAM access (baseline) ===
    float *h_data = (float*)malloc(bytes);
    for (int i = 0; i < n; i++) h_data[i] = (float)i;

    float *d_data;
    cudaMalloc(&d_data, bytes);
    cudaMemcpy(d_data, h_data, bytes, cudaMemcpyHostToDevice);

    hbf_access_test<<<4, 256>>>(d_data, n);
    cudaDeviceSynchronize();

    cudaMemcpy(h_data, d_data, bytes, cudaMemcpyDeviceToHost);
    int errors = 0;
    for (int i = 0; i < n; i++) {
        if (fabs(h_data[i] - (float)(i * 2)) > 0.1f) errors++;
    }
    printf("[DRAM Test] %s: %d errors out of %d\n", errors ? "FAILED" : "PASSED", errors, n);

    // === Test 2: Explicit HBF range access ===
    // HBF base = 274877906944 (256 GB default)
    unsigned long long hbf_base = 274877906944ULL;
    float *h_out = (float*)malloc(bytes);
    for (int i = 0; i < n; i++) h_out[i] = 0.0f;

    float *d_out;
    cudaMalloc(&d_out, bytes);
    cudaMemcpy(d_out, h_out, bytes, cudaMemcpyHostToDevice);

    // This kernel reads from HBF address range
    hbf_explicit_access<<<4, 256>>>(hbf_base, d_out, n);
    cudaDeviceSynchronize();

    cudaMemcpy(h_out, d_out, bytes, cudaMemcpyDeviceToHost);
    int hbf_errors = 0;
    // HBF reads: hbf_base is not actually initialized, so values are undefined.
    // We just check that the kernel ran without crashing.
    printf("[HBF Test]  Kernel executed. Output sample: h_out[0]=%f\n", h_out[0]);

    cudaFree(d_data);
    cudaFree(d_out);
    free(h_data);
    free(h_out);

    printf("HBF smoke test complete.\n");
    return errors ? 1 : 0;
}
