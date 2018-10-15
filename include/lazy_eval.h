#ifndef MOU_LAZY_EVAL_H
#define MOU_LAZY_EVAL_H

/*
 * \brief: refer to github.com/dmlc/mshadow
 */

#include <cstdio>
#include <type_traits>

namespace mou {

namespace expr {

/*
 * Expression Type
 * - Scalar
 * - Unary
 * - Binary
 * - Ternary
 */
template <typename EType>
struct Exp {
    // Exp<EType> -> EType
    inline const EType& self(void) const {
        return *static_cast<const EType*>(this);
    }
};

template <typename Ths>
struct ScalarMapExp : public Exp<ScalarMapExp<Ths> > {
    const Ths &hs;
    typedef Ths type;

    ScalarMapExp(const Ths &hs)
        : hs(hs) {}
    inline auto Eval(int i) const {
        return hs;
    }
};

template <typename OP, typename Ths>
struct UnaryMapExp : public Exp<UnaryMapExp<OP, Ths> > {
    const Ths &hs;
    typedef std::result_of_t<decltype(&OP::Map)(typename Ths::type)> type;

    UnaryMapExp(const Ths &hs)
        : hs(hs) {}
    inline auto Eval(int i) const {
        return OP::Map(hs.Eval(i));
    }
};

template <typename OP, typename Tlhs, typename Trhs>
struct BinaryMapExp : public Exp<BinaryMapExp<OP, Tlhs, Trhs> > {
    const Tlhs &lhs;
    const Trhs &rhs;
    typedef std::result_of_t<decltype(&OP::Map)(
                typename Tlhs::type,typename Trhs::type
            )> type;

    BinaryMapExp(const Tlhs &lhs, const Trhs &rhs)
        : lhs(lhs), rhs(rhs) {}

    inline auto Eval(int i) const {
        return OP::Map(lhs.Eval(i), rhs.Eval(i));
    }
};

template <typename OP, typename Tlhs, typename Tchs, typename Trhs>
struct TernaryMapExp : public Exp<TernaryMapExp<OP, Tlhs, Tchs, Trhs> > {
    const Tlhs &lhs;
    const Tchs &chs;
    const Trhs &rhs;
    typedef std::result_of_t<decltype(&OP::Map)(
                typename Tlhs::type,typename Tchs::type,typename Trhs::type
            )> type;

    TernaryMapExp(const Tlhs &lhs, const Tchs &chs, const Trhs &rhs)
        : lhs(lhs), chs(chs), rhs(rhs) {}

    inline auto Eval(int i) const {
        return OP::Map(lhs.Eval(i), chs.Eval(i), rhs.Eval(i));
    }
};

/*
 * Vector
 */
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

/*
 * Recursively get expression
 */
template <template<typename> class OP, typename Ths>
inline UnaryMapExp<OP<typename Ths::type>, Ths>
F(const Exp<Ths> &hs) {
    return UnaryMapExp<
                OP<typename Ths::type>, Ths
           >(hs.self());
}

template <template<typename, typename> class OP, typename Tlhs, typename Trhs>
inline BinaryMapExp<OP<typename Tlhs::type, typename Trhs::type>, Tlhs, Trhs>
F(const Exp<Tlhs> &lhs, const Exp<Trhs> &rhs) {
    return BinaryMapExp<
                OP<typename Tlhs::type, typename Trhs::type>, Tlhs, Trhs
           >(lhs.self(), rhs.self());
}

template <template<typename, typename, typename> class OP,
          typename Tlhs, typename Tchs, typename Trhs>
inline TernaryMapExp<
    OP<typename Tlhs::type, typename Tchs::type, typename Trhs::type>,
    Tlhs, Tchs, Trhs>
F(const Exp<Tlhs> &lhs, const Exp<Tchs> &chs, const Exp<Trhs> &rhs) {
    return TernaryMapExp<
                OP<typename Tlhs::type, typename Tchs::type, typename Trhs::type>,
                Tlhs, Tchs, Trhs
           >(lhs.self(), chs.self(), rhs.self());
}

/*
 * Unary Operator
 * - identity
 */
template <typename Ths>
struct identity {
    inline static auto Map(Ths a) {
        return a;
    }
};

/*
 * Binary Operator
 * - plus
 * - minus
 * - mul
 * - div
 */

template <typename Tlhs, typename Trhs>
struct plus {
    inline static auto Map(Tlhs a, Trhs b) {
        return a + b;
    }
};

template <typename Tlhs, typename Trhs>
struct minus {
    inline static auto Map(Tlhs a, Trhs b) {
        return a - b;
    }
};

template <typename Tlhs, typename Trhs>
struct mul {
    inline static auto Map(Tlhs a, Trhs b) {
        return a * b;
    }
};

template <typename Tlhs, typename Trhs>
struct div {
    inline static auto Map(Tlhs a, Trhs b) {
        return a / b;
    }
};

/*
 * Ternary Operator
 * - axpy
 */

template <typename Tlhs, typename Tchs, typename Trhs>
struct axpy {
    inline static auto Map(Tlhs a, Tchs x, Trhs y) {
        return a * x + y;
    }
};

/*
 * Wrapper for Binary Operator
 */
template <typename Tlhs, typename Trhs>
inline BinaryMapExp<plus<typename Tlhs::type, typename Trhs::type>, Tlhs, Trhs>
operator + (const Exp<Tlhs> &lhs, const Exp<Trhs> &rhs) {
    return F<plus>(lhs, rhs);
}

template <typename Tlhs,
          typename Trhs,
          bool T = std::is_scalar<Tlhs>::value,
          typename std::enable_if<T, Tlhs>::type* helper = nullptr>
inline BinaryMapExp<
    plus<typename ScalarMapExp<Tlhs>::type, typename Trhs::type>,
    ScalarMapExp<Tlhs>, Trhs>
operator + (const Tlhs &lhs, const Exp<Trhs> &rhs) {
    static auto expr = ScalarMapExp<Tlhs>(lhs);
    return F<plus>(expr, rhs);
}

template <typename Tlhs,
          typename Trhs,
          bool T = std::is_scalar<Trhs>::value,
          typename std::enable_if<T, Trhs>::type* helper = nullptr>
inline BinaryMapExp<
    plus<typename Tlhs::type, typename ScalarMapExp<Trhs>::type>,
    Tlhs, ScalarMapExp<Trhs> >
operator + (const Exp<Tlhs> &lhs, const Trhs &rhs) {
    static auto expr = ScalarMapExp<Trhs>(rhs);
    return F<plus>(lhs, expr);
}

template <typename Tlhs, typename Trhs>
inline BinaryMapExp<minus<typename Tlhs::type, typename Trhs::type>, Tlhs, Trhs>
operator - (const Exp<Tlhs> &lhs, const Exp<Trhs> &rhs) {
    return F<minus>(lhs, rhs);
}

template <typename Tlhs,
          typename Trhs,
          bool T = std::is_scalar<Tlhs>::value,
          typename std::enable_if<T, Tlhs>::type* helper = nullptr>
inline BinaryMapExp<
    minus<typename ScalarMapExp<Tlhs>::type, typename Trhs::type>,
    ScalarMapExp<Tlhs>, Trhs>
operator - (const Tlhs &lhs, const Exp<Trhs> &rhs) {
    static auto expr = ScalarMapExp<Tlhs>(lhs);
    return F<minus>(expr, rhs);
}

template <typename Tlhs,
          typename Trhs,
          bool T = std::is_scalar<Trhs>::value,
          typename std::enable_if<T, Trhs>::type* helper = nullptr>
inline BinaryMapExp<
    minus<typename Tlhs::type, typename ScalarMapExp<Trhs>::type>,
    Tlhs, ScalarMapExp<Trhs> >
operator - (const Exp<Tlhs> &lhs, const Trhs &rhs) {
    static auto expr = ScalarMapExp<Trhs>(rhs);
    return F<minus>(lhs, expr);
}

template <typename Tlhs, typename Trhs>
inline BinaryMapExp<mul<typename Tlhs::type, typename Trhs::type>, Tlhs, Trhs>
operator * (const Exp<Tlhs> &lhs, const Exp<Trhs> &rhs) {
    return F<mul>(lhs, rhs);
}

template <typename Tlhs,
          typename Trhs,
          bool T = std::is_scalar<Tlhs>::value,
          typename std::enable_if<T, Tlhs>::type* helper = nullptr>
inline BinaryMapExp<
    mul<typename ScalarMapExp<Tlhs>::type, typename Trhs::type>,
    ScalarMapExp<Tlhs>, Trhs>
operator * (const Tlhs &lhs, const Exp<Trhs> &rhs) {
    static auto expr = ScalarMapExp<Tlhs>(lhs);
    return F<mul>(expr, rhs);
}

template <typename Tlhs,
          typename Trhs,
          bool T = std::is_scalar<Trhs>::value,
          typename std::enable_if<T, Trhs>::type* helper = nullptr>
inline BinaryMapExp<
    mul<typename Tlhs::type, typename ScalarMapExp<Trhs>::type>,
    Tlhs, ScalarMapExp<Trhs> >
operator * (const Exp<Tlhs> &lhs, const Trhs &rhs) {
    static auto expr = ScalarMapExp<Trhs>(rhs);
    return F<mul>(lhs, expr);
}

template <typename Tlhs, typename Trhs>
inline BinaryMapExp<div<typename Tlhs::type, typename Trhs::type>, Tlhs, Trhs>
operator / (const Exp<Tlhs> &lhs, const Exp<Trhs> &rhs) {
    return F<div>(lhs, rhs);
}

template <typename Tlhs,
          typename Trhs,
          bool T = std::is_scalar<Tlhs>::value,
          typename std::enable_if<T, Tlhs>::type* helper = nullptr>
inline BinaryMapExp<
    div<typename ScalarMapExp<Tlhs>::type, typename Trhs::type>,
    ScalarMapExp<Tlhs>, Trhs>
operator / (const Tlhs &lhs, const Exp<Trhs> &rhs) {
    static auto expr = ScalarMapExp<Tlhs>(lhs);
    return F<div>(expr, rhs);
}

template <typename Tlhs,
          typename Trhs,
          bool T = std::is_scalar<Trhs>::value,
          typename std::enable_if<T, Trhs>::type* helper = nullptr>
inline BinaryMapExp<
    div<typename Tlhs::type, typename ScalarMapExp<Trhs>::type>,
    Tlhs, ScalarMapExp<Trhs> >
operator / (const Exp<Tlhs> &lhs, const Trhs &rhs) {
    static auto expr = ScalarMapExp<Trhs>(rhs);
    return F<div>(lhs, expr);
}

} // namespace expr

} // namespace mou

#endif // MOU_LAZY_EVAL_H
