#include "../include/mou/tensor_gpu.h"
#include <iostream>

using namespace mou::tensor;

void test_tensor_gpu() {
    Tensor<int, device::gpu, 0, gpu_allocator<int>> a({1, 2, 3});
    std::cout << a << std::endl;
}

int main() {
    test_tensor_gpu();

    return 0;
}
