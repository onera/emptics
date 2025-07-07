#include <cstdio>

__global__
void kernel_hello_world() {
  printf("Hello World from GPU!\n");
}


int main() {
  kernel_hello_world<<<1,1>>>();
  cudaDeviceSynchronize(); // make sure the kernel has finished
  cudaError_t error = cudaGetLastError();
  if (error != cudaSuccess) {
    printf("Error during GPU computation. Error code: %i, Error string: %s\n",error, cudaGetErrorString(error));
  }
  return 0;
}
