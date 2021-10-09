
#include "cuda_func.h"
#include <iostream>
#define CUDA_LAUNCH_BLOCKING 1
__global__ void test_kernel(int* first, int* second, const size_t size)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    if (index < size)
        first[index] += second[index];
}
namespace CUDA {
    void helloWorld(void) {
        const size_t size = 45;
        int first[size];
        int second[size];
        for (int i = 0; i < size; i++) {
            first[i] = i + 5;
            second[i] = i - 2;
        }
        const size_t mem_size = sizeof(first);
        int *d_first;
        cudaMalloc(&d_first, mem_size);
        int *d_second;
        cudaMalloc(&d_second, mem_size);

        cudaMemcpy(d_first, first, mem_size, cudaMemcpyHostToDevice);
        cudaMemcpy(d_second, second, mem_size, cudaMemcpyHostToDevice);


        test_kernel <<<6, 9>>> (d_first, d_second, size);

        cudaMemcpy(first, d_first, mem_size, cudaMemcpyDeviceToHost);

        for (int i = 0; i < size; i++)
            std::cout << first[i] << " ";
        std::cout << std::endl;
    }
}