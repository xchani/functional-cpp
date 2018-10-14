#ifndef MOU_LAZY_EVAL_H
#define MOU_LAZY_EVAL_H

/*
 * \brief: refer to github.com/dmlc/mshadow
 */

#include <cstdio>
#include <type_traits>

namespace mou {

namespace expr {

template <typename EType>
struct Exp {
    // Exp<EType> -> EType
    inline const EType& self(void) const {
        return *static_cast<const EType*>(this);
    }
};

template <typename OP, typename Tlhs, typename Trhs>
struct BinaryMapExp : public Exp<BinaryMapExp<OP, Tlhs, Trhs> > {
    const Tlhs &lhs;
    const Trhs &rhs;
    typedef std::result_of_t<decltype(&OP::Map)(typename Tlhs::type,typename Trhs::type)> type;

    BinaryMapExp(const Tlhs &lhs, const Trhs &rhs)
        : lhs(lhs), rhs(rhs) {}

    inline auto Eval(int i) const {
        return OP::Map(lhs.Eval(i), rhs.Eval(i));
    }
};

template <typename DType>
struct Vec : public Exp<Vec<DType> > {
    DType *dptr;
    size_t len;
    typedef DType type;

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

template <template<typename, typename> class OP, typename Tlhs, typename Trhs>
inline BinaryMapExp<OP<typename Tlhs::type, typename Trhs::type>, Tlhs, Trhs>
F(const Exp<Tlhs> &lhs, const Exp<Trhs> &rhs) {
    return BinaryMapExp<
                OP<typename Tlhs::type, typename Trhs::type>, Tlhs, Trhs
           >( lhs.self(), rhs.self());
}

template <typename Tlhs, typename Trhs>
struct mul {
    inline static auto Map(Tlhs a, Trhs b) {
        return a * b;
    }
};

template <typename Tlhs, typename Trhs>
inline BinaryMapExp<mul<typename Tlhs::type, typename Trhs::type>, Tlhs, Trhs>
operator * (const Exp<Tlhs> &lhs, const Exp<Trhs> &rhs) {
    return F<mul>(lhs, rhs);
}

template <typename Tlhs, typename Trhs>
struct div {
    inline static auto Map(Tlhs a, Trhs b) {
        return a / b;
    }
};

template <typename Tlhs, typename Trhs>
inline BinaryMapExp<div<typename Tlhs::type, typename Trhs::type>, Tlhs, Trhs>
operator / (const Exp<Tlhs> &lhs, const Exp<Trhs> &rhs) {
    return F<div>(lhs, rhs);
}

} // namespace expr

} // namespace mou

#endif // MOU_LAZY_EVAL_H
