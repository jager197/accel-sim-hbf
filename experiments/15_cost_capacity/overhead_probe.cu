#include <cuda_runtime.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>

__global__ void vector_add(const float *a, const float *b, float *c, int n) {
  int index = blockIdx.x * blockDim.x + threadIdx.x;
  if (index < n) c[index] = a[index] + b[index];
}

int main(int argc, char **argv) {
  int iterations = argc > 1 ? std::atoi(argv[1]) : 32;
  int elements = argc > 2 ? std::atoi(argv[2]) : 4096;
  if (iterations <= 0 || elements <= 0 || elements > (1 << 20)) {
    std::fprintf(stderr, "iterations and elements must be positive\n");
    return 2;
  }

  size_t bytes = static_cast<size_t>(elements) * sizeof(float);
  float *host_a = static_cast<float *>(std::malloc(bytes));
  float *host_b = static_cast<float *>(std::malloc(bytes));
  float *host_c = static_cast<float *>(std::malloc(bytes));
  if (host_a == nullptr || host_b == nullptr || host_c == nullptr) return 1;
  for (int index = 0; index < elements; ++index) {
    host_a[index] = std::sin(static_cast<float>(index));
    host_b[index] = std::cos(static_cast<float>(index));
  }

  float *device_a = nullptr;
  float *device_b = nullptr;
  float *device_c = nullptr;
  if (cudaMalloc(&device_a, bytes) != cudaSuccess ||
      cudaMalloc(&device_b, bytes) != cudaSuccess ||
      cudaMalloc(&device_c, bytes) != cudaSuccess ||
      cudaMemcpy(device_a, host_a, bytes, cudaMemcpyHostToDevice) != cudaSuccess ||
      cudaMemcpy(device_b, host_b, bytes, cudaMemcpyHostToDevice) != cudaSuccess) {
    std::fprintf(stderr, "CUDA allocation or copy failed\n");
    return 1;
  }

  int threads = 256;
  int blocks = (elements + threads - 1) / threads;
  for (int iteration = 0; iteration < iterations; ++iteration)
    vector_add<<<blocks, threads>>>(device_a, device_b, device_c, elements);
  if (cudaDeviceSynchronize() != cudaSuccess ||
      cudaMemcpy(host_c, device_c, bytes, cudaMemcpyDeviceToHost) != cudaSuccess) {
    std::fprintf(stderr, "CUDA execution or copy failed\n");
    return 1;
  }

  int errors = 0;
  for (int index = 0; index < elements; ++index) {
    float expected = host_a[index] + host_b[index];
    if (std::fabs(host_c[index] - expected) > 1e-5f) ++errors;
  }
  std::printf("[OVERHEAD-PROBE] completed: %d iterations x %d elements, %d errors\n",
              iterations, elements, errors);

  cudaFree(device_a);
  cudaFree(device_b);
  cudaFree(device_c);
  std::free(host_a);
  std::free(host_b);
  std::free(host_c);
  return errors == 0 ? 0 : 1;
}
