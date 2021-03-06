#ifndef MOU_TENSOR_H
#define MOU_TENSOR_H

#include "expression.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <memory>
#include <numeric>
#include <ostream>
#include <type_traits>
#include <utility>
#include <vector>

namespace mou {

namespace tensor {

template <typename DType>
class ShapeBase {
 public:
    template <typename... T>
    explicit ShapeBase(T ...args) {
        shape = {static_cast<DType>(args)...};
        len = (... * static_cast<DType>(args));
    }

    explicit ShapeBase() {
        shape = {};
        len = 0;
    }

    ShapeBase(std::initializer_list<DType> l) {
        shape = l;
        len =  _get_shape_length(shape);
    }

    explicit ShapeBase(std::vector<size_t> src) {
        shape = src;
        len =  _get_shape_length(shape);
    }

    DType operator [] (size_t i) const {
        return shape.at(i);
    }

    bool operator == (const ShapeBase& other) const {
        return shape == other.shape;
    }

    ShapeBase& operator = (std::initializer_list<DType> l) {
        shape = l;
        len =  _get_shape_length(shape);
        return *this;
    }

    const auto begin() const noexcept {
        return shape.begin();
    }

    const auto end() const noexcept {
        return shape.end();
    }

    const auto rbegin() const noexcept {
        return shape.rbegin();
    }

    const auto rend() const noexcept {
        return shape.rend();
    }

    inline DType Dims() const {
        return shape.size();
    }

    inline DType Size() const {
        return len;
    }

    inline DType Size(size_t i) const {
        return shape.at(i);
    }

    inline DType Size(size_t start, size_t end) const {
        DType sz = 1;
        for (size_t i = start; i < end; ++i) {
            sz *= shape.at(i);
        }
        return sz;
    }

    inline DType SizeFrom(size_t start) const {
        return this->Size(start, this->Dims());
    }

    inline DType SizeTo(size_t end) const {
        return this->Size(0, end);
    }

    friend std::ostream& operator << (std::ostream& os, const ShapeBase& other) {
        os << '(';
        for (size_t i = 0; i < other.Dims(); ++i) {
            if (i != 0) os << ',';
            os << other.Size(i);
        }
        if (other.Dims() == 1) os << ',';
        os << ')';
        return os;
    }

 private:
    inline DType _get_shape_length(std::vector<DType> _shape) const {
        return std::accumulate(_shape.begin(), _shape.end(),
                               1, std::multiplies<>());
    }

 private:
    std::vector<DType> shape;
    DType len;
};

using Shape = ShapeBase<size_t>;

enum device { cpu, gpu };

std::ostream& operator << (std::ostream& os, device dev) {
    switch (dev) {
        case cpu    : os << "cpu"; break;
        case gpu    : os << "gpu"; break;
        default     : os.setstate(std::ios_base::failbit);
    }
    return os;
}

template <typename InputIt, typename ForwardIt, int InputDevType, int ForwardDevType>
struct uninit_copy_to_dev;

template <typename InputIt, typename ForwardIt>
struct uninit_copy_to_dev<InputIt, ForwardIt, device::cpu, device::cpu> {
    static ForwardIt Copy(InputIt first, InputIt last, ForwardIt d_first) {
        return std::uninitialized_copy(first, last, d_first);
    }
};

template <typename InputIt, typename ForwardIt, int InputDevType, int ForwardDevType>
struct copy_to_dev;

template <typename InputIt, typename ForwardIt>
struct copy_to_dev<InputIt, ForwardIt, device::cpu, device::cpu> {
    static ForwardIt Copy(InputIt first, InputIt last, ForwardIt d_first) {
        return std::copy(first, last, d_first);
    }
};

template <typename DType, int DevType=device::cpu, int DevId=0,
          typename Allocator = std::allocator<DType> >
class Tensor : public expr::Exp<Tensor<DType> > {
 public:
    using type = DType;
    device dev = static_cast<device>(DevType);
    int dev_id = DevId;

 private:
    using pointer = typename std::allocator_traits<Allocator>::pointer;

    pointer dptr;
    Allocator alloc;
    Shape shape;

 public:
    explicit Tensor(Shape _shape) : shape(std::move(_shape)) {
        dptr = _M_allocate(this->Size());
    }

    // TODO(Chenxia Han): Constructor with nested initializer_list
    Tensor(std::initializer_list<DType> l) {
        shape = {l.size()};
        // initializer_list must store in cpu
        dptr = _M_allocate_and_copy<device::cpu>(l.begin(), l.end());
    }

    // only support memory resource with the same device type
    explicit Tensor(pointer first, pointer last, Shape _shape)
        : shape(std::move(_shape)) {
        assert(last-first == _shape.Size());
        dptr = _M_allocate_and_copy<DevType>(first, last);
    }

    ~Tensor() {
        _M_deallocate(dptr, this->Size());
    }

    // only support tensor in the same device type
    Tensor(const Tensor& src) {
        shape = src.shape;
        dptr = _M_allocate_and_copy<DevType>(src.dptr, src.dptr + src.Size());
    }

    Tensor(Tensor&& src) noexcept {
        shape = std::move(src.shape);
        dptr = _M_allocate_on_move(src.dptr);
    }

    // only support tensor in the same device type
    Tensor& operator = (const Tensor& src) {
        if (shape == src.shape) {
            _M_copy<DevType>(src.dptr, src.dptr + src.Size(), dptr);
        } else {
            _M_deallocate(dptr, this->Size());
            shape = src.shape;
            dptr = _M_allocate_and_copy<DevType>(src.dptr, src.dptr + src.Size());
        }

        return *this;
    }

    Tensor& operator = (Tensor&& src) noexcept {
        _M_deallocate(dptr, this->Size());
        shape = std::move(src.shape);
        dptr = _M_allocate_on_move(src.dptr);

        return *this;
    }

    Tensor& operator = (std::initializer_list<DType> l) {
        if (shape == Shape(l.size())) {
            _M_copy<device::cpu>(l.begin(), l.end(), dptr);
        } else {
            _M_deallocate(dptr, this->Size());
            shape = {l.size()};
            // initializer_list must store in cpu
            dptr = _M_allocate_and_copy<device::cpu>(l.begin(), l.end());
        }

        return *this;
    }

    DType& operator [] (size_t i) const {
        return dptr[i];
    }

    template <typename EType>
    inline Tensor& operator = (const expr::Exp<EType> &src) {
        const EType &src_ = src.self();
        for (int i = 0; i < this->Size(); ++i) {
            dptr[i] = src_.Eval(i);
        }
        return *this;
    }

    template <typename EType,
              typename = typename std::enable_if_t<std::is_scalar_v<EType>>>
    inline Tensor& operator = (const EType &src) {
        auto expr = expr::ScalarMapExp<EType>(src);
        (*this) = expr;
        return *this;
    }

    inline DType Eval(int i) const {
        return dptr[i];
    }

    inline Shape Shape_() const {
        return shape;
    }

    inline size_t Size() const {
        return shape.Size();
    }

    void Reshape(const Shape& src) {
        assert(this->Size() == src.Size());
        shape = src;
    }

    template <typename... T>
    void Reshape(T ...args) {
        Shape src = Shape(args...);
        assert(this->Size() == src.Size());
        shape = src;
    }

    template <typename T>
    void ReshapeLike(const Tensor<T>& other) {
        assert(this->Size() == other.Size());
        shape = other.shape;
    }

    Tensor Slice(size_t begin, size_t end) const {
        size_t slice_len = end - begin;
        assert(slice_len > 0 && begin >= 0);

        auto n = shape.SizeFrom(1);
        std::vector<size_t> slice_shape;
        slice_shape.push_back(slice_len);
        for (int i = 1; i < shape.Dims(); ++i) {
            slice_shape.push_back(shape[i]);
        }

        return Tensor(dptr + begin * n, dptr + end * n, Shape(slice_shape));
    }

    inline Tensor SliceFrom(size_t begin) const {
        return this->Slice(begin, this->shape[0]);
    }

    inline Tensor SliceTo(size_t end) const {
        return this->Slice(0, end);
    }

    friend std::ostream& operator << (std::ostream& os, const Tensor& other) {
        const Shape& shape = other.Shape_();
        std::vector<size_t> cumsum(shape.Dims());
        std::partial_sum(shape.rbegin(), shape.rend(), cumsum.rbegin(), std::multiplies<>());

        // count width for each element in tensor
        size_t width = 0;
        for (size_t i = 0; i < other.Size(); ++i) {
            auto int_num = std::floor(std::abs(other[i]));
            int_num = std::max(int_num, static_cast<decltype(int_num)>(1));
            size_t int_width = std::log10(int_num) + 1;
            int_width += (other[i] < 0);
            width = std::max(width, int_width + 1);
        }
        if (std::is_floating_point_v<type>) {
            // another one for decimal mark
            width += os.precision() + 1;
        }

        for (size_t i = 0; i < other.Size(); ++i) {
            bool begin_of_dim = false;
            bool end_of_dim = false;
            bool end_of_tensor = (i == other.Size() - 1);

            // n left brackets if i % (D_(n-1) * D_(n-2) * ... * D_0) = 0
            for (auto v : cumsum) {
                if (i % v == 0) begin_of_dim = true;
            }
            if (begin_of_dim) {
                for (auto v : cumsum) {
                    if (i % v == 0) os << '[';
                    else os << ' ';
                }
            }

            os << std::setw(width) << other[i];

            // n right brackets if i % (D_(n-1) * D_(n-2) * ... * D_0) = 0
            for (auto v : cumsum) {
                if (i % v == v - 1) {
                    end_of_dim = true;
                    os << ']';
                }
            }
            if (!end_of_tensor) {
                os << ',';
                if (end_of_dim) os << '\n';
            }
        }

        // special for empty tensor
        if (other.Size() == 0) os << "[]";

        os << '\n';

        // print shape of tensor
        // example: <NDArray 2x3x4 @cpu(0)>
        os << '<' << "NDArray" << ' ';
        for (size_t i = 0; i < shape.Dims(); ++i) {
            if (i != 0) os << 'x';
            os << shape[i];
        }
        // TODO(Chenxia Han): print device type and id
        os << ' ' << '@' << other.dev << '(' << other.dev_id << ')' << '>';
        os << std::endl;

        return os;
    }

    friend DType Dot(const Tensor& a, const Tensor& b) {
        assert(a.Size() == b.Size());
        DType sum = 0;
        for (size_t i = 0; i < a.Size(); ++i) {
            sum += a[i] * b[i];
        }
        return sum;
    }

 private:
    pointer _M_allocate(size_t n) {
        using alloc_traits = std::allocator_traits<Allocator>;
        return n != 0 ? alloc_traits::allocate(alloc, n) : pointer();
    }

    void _M_deallocate(pointer p, size_t n) {
        using alloc_traits = std::allocator_traits<Allocator>;
        if (p) {
            alloc_traits::deallocate(alloc, p, n);
        }
    }

    template <int ForwardDevType, typename ForwardIterator>
    pointer _M_allocate_and_copy(ForwardIterator first,
            ForwardIterator last) {
        auto n = last - first;
        pointer result = _M_allocate(n);
        try {
            uninit_copy_to_dev<ForwardIterator, pointer, ForwardDevType, DevType>
                ::Copy(first, last, result);
            return result;
        } catch(...) {
            _M_deallocate(result, n);
            std::exception_ptr eptr = std::current_exception();
            std::rethrow_exception(eptr);
        }
    }

    template <typename ForwardIterator>
    pointer _M_allocate_on_move(ForwardIterator& first) {
        pointer result = first;
        first = nullptr;
        return result;
    }

    template <int ForwardDevType, typename ForwardIterator>
    void _M_copy(ForwardIterator first,
            ForwardIterator last, pointer result) {
        copy_to_dev<ForwardIterator, pointer, ForwardDevType, DevType>
            ::Copy(first, last, result);
    }
};

}; // namespace tensor

}; // namespace mou

#endif // MOU_TENSOR_H
