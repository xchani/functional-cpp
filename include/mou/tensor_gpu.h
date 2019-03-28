#ifndef MOU_TENSOR_GPU_H
#define MOU_TENSOR_GPU_H

#include "tensor.h"
#include <memory>

namespace mou {

namespace tensor {

template <class T>
class gpu_allocator : public std::allocator<T> {
 public:
    using size_type = typename std::allocator<T>::size_type;
    using pointer = typename std::allocator<T>::pointer;

    pointer allocate(size_type n, std::allocator<void>::const_pointer hint = 0) {
        if (n > max_size()) {
            // TODO(Chenxia Han): use LOG(ERROR) instead
            printf("gpu_allocator<T>::allocate(size_t n)"
                " 'n' exceeds maximum supported size");
        }
        pointer device_pointer;
        cudaMalloc((void**)&device_pointer, n * sizeof(T));
        return device_pointer;
    }
    void deallocate(pointer p, size_type n) noexcept {
        cudaFree(p);
    }
    inline size_type max_size() const noexcept {
        return std::allocator<T>::max_size();
    }
};

// uninitialized copy from cpu to gpu
template <typename InputIt, typename ForwardIt>
struct uninit_copy_to_dev<InputIt, ForwardIt, device::cpu, device::gpu> {
    static ForwardIt Copy(InputIt first, InputIt last, ForwardIt d_first) {
        size_t copy_bytes = (last-first) * sizeof(typename std::iterator_traits<InputIt>::value_type);
        cudaMemcpy(d_first, first, copy_bytes, cudaMemcpyHostToDevice);
        return d_first;
    }
};

// uninitialized copy from gpu to gpu
template <typename InputIt, typename ForwardIt>
struct uninit_copy_to_dev<InputIt, ForwardIt, device::gpu, device::gpu> {
    static ForwardIt Copy(InputIt first, InputIt last, ForwardIt d_first) {
        size_t copy_bytes = (last-first) * sizeof(typename std::iterator_traits<InputIt>::value_type);
        cudaMemcpy(d_first, first, copy_bytes, cudaMemcpyDeviceToDevice);
        return d_first;
    }
};

// copy from cpu to gpu
template <typename InputIt, typename ForwardIt>
struct copy_to_dev<InputIt, ForwardIt, device::cpu, device::gpu> {
    static ForwardIt Copy(InputIt first, InputIt last, ForwardIt d_first) {
        size_t copy_bytes = (last-first) * sizeof(typename std::iterator_traits<InputIt>::value_type);
        cudaMemcpy(d_first, first, copy_bytes, cudaMemcpyHostToDevice);
        return d_first;
    }
};

// copy from gpu to gpu
template <typename InputIt, typename ForwardIt>
struct copy_to_dev<InputIt, ForwardIt, device::gpu, device::gpu> {
    static ForwardIt Copy(InputIt first, InputIt last, ForwardIt d_first) {
        size_t copy_bytes = (last-first) * sizeof(typename std::iterator_traits<InputIt>::value_type);
        cudaMemcpy(d_first, first, copy_bytes, cudaMemcpyDeviceToDevice);
        return d_first;
    }
};

}; // namespace tensor

}; // namespace mou

#endif // MOU_TENSOR_GPU_H
