#ifndef MOU_TENSOR_H
#define MOU_TENSOR_H

#include "lazy_eval.h"
#include <cassert>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <numeric>
#include <ostream>
#include <utility>
#include <vector>

namespace mou {

namespace tensor {

// TODO(Chenxia Han): Deduce template argument in function prototype
template <typename DType = size_t>
class Shape {
 public:
    template <typename... T>
    explicit Shape(T ...args) {
        shape = {static_cast<DType>(args)...};
        len = (... * static_cast<DType>(args));
    }

    explicit Shape() {
        shape = {};
        len = 0;
    }

    Shape(std::initializer_list<DType> l) {
        shape = l;
        len = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<>());
    }

    // TODO(Chenxia Han): Update len after modifying shape
    DType operator [] (size_t i) const {
        return shape.at(i);
    }

    bool operator == (const Shape& other) const {
        return shape == other.shape;
    }

    Shape& operator = (std::initializer_list<DType> l) {
        shape = l;
        len = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<>());
        return *this;
    }

    inline DType Dims() const {
        return shape.size();
    }

    inline DType Size() const {
        //len = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<>());
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

    friend std::ostream& operator << (std::ostream& os, const Shape<>& other) {
        os << '(';
        for (size_t i = 0; i < other.shape.size(); ++i) {
            os << other.shape.at(i) << ",)"[i==other.shape.size()-1];
        }
        return os;
    }

 private:
    std::vector<DType> shape;
    DType len;
};

template <typename DType>
class Tensor : public expr::Exp<Tensor<DType> > {
 public:
    explicit Tensor(Shape<> shape) : shape(shape) {
        // TODO(Chenxia Han): Try to dismiss raw pointer
        dptr = static_cast<DType*>(std::malloc(sizeof(DType) * shape.Size()));
    }

    // TODO(Chenxia Han): Constructor with nested initializer_list
    Tensor(std::initializer_list<DType> l) {
        shape = {l.size()};
        dptr = static_cast<DType*>(std::malloc(sizeof(DType) * shape.Size()));
        std::copy(l.begin(), l.end(), dptr);
    }

    ~Tensor() {
        if (dptr != nullptr) {
            std::free(dptr);
        }
    }

    // Copy Constructor
    Tensor(const Tensor& src) {
        shape = src.shape;

        dptr = static_cast<DType*>(std::malloc(sizeof(DType) * shape.Size()));
        std::memcpy(dptr, src.dptr, sizeof(DType) * shape.Size());
    }

    // Move Constructor
    Tensor(Tensor&& src) noexcept {
        shape = std::move(src.shape);

        dptr = static_cast<DType*>(std::malloc(sizeof(DType) * shape.Size()));
        dptr = src.dptr;
        src.dptr = nullptr;
    }

    // Copy Assignment
    Tensor& operator = (const Tensor& src) {
        // TODO(Chenxia Han): Check shape in compile time
        if (shape == src.shape) {
            shape = src.shape;
        } else {
            shape = src.shape;
            if (dptr != nullptr) {
                std::free(dptr);
            }
            dptr = static_cast<DType*>(std::malloc(sizeof(DType) * shape.Size()));
        }
        std::memcpy(dptr, src.dptr, sizeof(DType) * shape.Size());

        return *this;
    }

    // Move Assignment
    Tensor& operator = (Tensor&& src) noexcept {
        if (dptr != nullptr) {
            std::free(dptr);
        }
        shape = std::move(src.shape);

        dptr = src.dptr;
        src.dptr = nullptr;

        return *this;
    }

    Tensor& operator = (std::initializer_list<DType> l) {
        if (dptr != nullptr) {
            std::free(dptr);
        }
        shape = {l.size()};
        dptr = static_cast<DType*>(std::malloc(sizeof(DType) * shape.Size()));
        std::copy(l.begin(), l.end(), dptr);
        return *this;
    }

    DType operator [] (size_t i) const {
        return dptr[i];
    }

    template <typename EType>
    inline Tensor& operator = (const expr::Exp<EType> &src) {
        const EType &src_ = src.self();
        for (int i = 0; i < shape.Size(); ++i) {
            dptr[i] = src_.Eval(i);
        }
        return *this;
    }

    inline DType Eval(int i) const {
        return dptr[i];
    }

    inline Shape<> Shape_() const {
        return shape;
    }

    void Reshape(const Shape<>& src) {
        assert(shape.Size() == src.Size());
        shape = src;
    }

    template <typename T>
    void ReshapeLike(const Tensor<T>& other) {
        assert(shape.Size() == other.shape.Size());
        shape = other.shape;
    }

    friend std::ostream& operator << (std::ostream& os, const Tensor& other) {
        // TODO(Chenxia Han): Format output via dimensions
        os << '[';
        for (size_t i = 0; i < other.shape.Size(); ++i) {
            os << other[i] << ",]"[i==other.shape.Size()-1];
        }
        return os;
    }

 private:
    DType *dptr;
    Shape<> shape;
    using type = DType;
};

}; // namespace tensor

}; // namespace mou

#endif // MOU_TENSOR_H
