#ifndef LAZY_EVAL_H
#define LAZY_EVAL_H

/*
 * \brief: refer to github.com/dmlc/mshadow
 */

#include <cstdio>

template <typename EType>
struct Exp {
    // Exp<EType> -> EType
    inline const EType& self(void) const {
        return *static_cast<const EType*>(this);
    }
};

template <typename DType, typename OP, typename Tlhs, typename Trhs>
struct BinaryMapExp : public Exp<BinaryMapExp<DType, OP, Tlhs, Trhs> > {
    const Tlhs &lhs;
    const Trhs &rhs;

    BinaryMapExp(const Tlhs &lhs, const Trhs &rhs)
        : lhs(lhs), rhs(rhs) {}

    inline DType Eval(int i) const {
        return OP::Map(lhs.Eval(i), rhs.Eval(i));
    }
};

template <typename DType>
struct Vec : public Exp<Vec<DType> > {
    DType *dptr;
    size_t len;

    Vec(DType *dptr, size_t len) : dptr(dptr), len(len) {} 

    template <typename EType>
    inline Vec& operator = (const Exp<EType> &src) {
        const EType &src_ = src.self();
        for (int i = 0; i < len; ++i) {
            dptr[i] = src_.Eval(i);
        }
        return *this;
    }

    inline DType Eval(int i) const {
        return dptr[i];
    }
};

template <typename DType, typename OP, typename Tlhs, typename Trhs>
inline BinaryMapExp<DType, OP, Tlhs, Trhs>
F(const Exp<Tlhs> &lhs, const Exp<Trhs> &rhs) {
    return BinaryMapExp<DType, OP, Tlhs, Trhs>(lhs.self(), rhs.self());
}

struct mul {
    inline static float Map(float a, float b) {
        return a * b;
    }
};

template <typename DType, typename Tlhs, typename Trhs>
inline BinaryMapExp<DType, mul, Tlhs, Trhs>
operator * (const Exp<Tlhs> &lhs, const Exp<Trhs> &rhs) {
    return F<DType, mul>(lhs, rhs);
}

struct div {
    inline static int Map(int a, int b) {
        return a / b;
    }
};

template <typename DType, typename Tlhs, typename Trhs>
inline BinaryMapExp<DType, div, Tlhs, Trhs>
operator / (const Exp<Tlhs> &lhs, const Exp<Trhs> &rhs) {
    return F<DType, div>(lhs, rhs);
}

#endif // LAZY_EVAL_H
