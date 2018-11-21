#ifndef MOU_EXPRESSION_H
#define MOU_EXPRESSION_H

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
    inline const EType& self() const {
        return *static_cast<const EType*>(this);
    }
};

template <typename Ths>
struct ScalarMapExp : public Exp<ScalarMapExp<Ths> > {
    const Ths &hs;
    using type = Ths;

    explicit ScalarMapExp(const Ths &hs)
        : hs(hs) {}
    inline auto Eval(int /*i*/) const {
        return hs;
    }
};

template <typename OP, typename Ths>
struct UnaryMapExp : public Exp<UnaryMapExp<OP, Ths> > {
    const Ths &hs;
    using type = std::result_of_t<decltype(&OP::Map) (typename Ths::type)>;

    explicit UnaryMapExp(const Ths &hs)
        : hs(hs) {}
    inline auto Eval(int i) const {
        return OP::Map(hs.Eval(i));
    }
};

template <typename OP, typename Tlhs, typename Trhs>
struct BinaryMapExp : public Exp<BinaryMapExp<OP, Tlhs, Trhs> > {
    const Tlhs &lhs;
    const Trhs &rhs;
    using type = std::result_of_t<decltype(&OP::Map) (
                typename Tlhs::type, typename Trhs::type
            )>;

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
    using type = std::result_of_t<decltype(&OP::Map) (
                typename Tlhs::type, typename Tchs::type, typename Trhs::type
            )>;

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
    using type = DType;

    Vec(DType *dptr, size_t len) : dptr(dptr), len(len) {} 

    template <typename EType>
    inline Vec& operator = (const Exp<EType> &src) {
        const EType &src_ = src.self();
        for (int i = 0; i < len; ++i) {
            dptr[i] = src_.Eval(i);
        }
        return *this;
    }

    template <
        typename EType,
        typename = typename std::enable_if_t<std::is_scalar_v<EType>>>
    inline Vec& operator = (const EType &src) {
        auto expr = ScalarMapExp<EType>(src);
        (*this) = expr;
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
 * Wrapper macro for Unary Operator
 */
#define DECLARE_UNARY_OP(OP_NAME, OP_EXP)                                   \
    template <typename Ths>                                                 \
    struct OP_NAME {                                                        \
        inline static auto Map(Ths a) {                                     \
            return OP_EXP;                                                  \
        }                                                                   \
    };

/*
 * Wrapper macro for Binary Operator
 */
#define DECLARE_BINARY_OP(OP_NAME, OP_EXP)                                  \
    template <typename Tlhs, typename Trhs>                                 \
    struct OP_NAME {                                                        \
        inline static auto Map(Tlhs a, Trhs b) {                            \
            return OP_EXP;                                                  \
        }                                                                   \
    };

/*
 * Wrapper macro for Ternary Operator
 */
#define DECLARE_TERNARY_OP(OP_NAME, OP_EXP)                                 \
    template <typename Tlhs, typename Tchs, typename Trhs>                  \
    struct OP_NAME {                                                        \
        inline static auto Map(Tlhs a, Tchs b, Trhs c) {                    \
            return OP_EXP;                                                  \
        }                                                                   \
    };

/*
 * Wrapper macro for Binary Operator Symbol
 */
#define DECLARE_BINARY_OP_SYM(OP_NAME, OP_SYM)                              \
    template <typename Tlhs, typename Trhs>                                 \
    inline BinaryMapExp<                                                    \
        OP_NAME<typename Tlhs::type, typename Trhs::type>,                  \
        Tlhs,                                                               \
        Trhs>                                                               \
    operator OP_SYM (const Exp<Tlhs> &lhs, const Exp<Trhs> &rhs) {          \
        return F<OP_NAME>(lhs, rhs);                                        \
    }

#define DECLARE_BINARY_OP_SYM_WITH_SCALAR_LEFT(OP_NAME, OP_SYM)             \
    template <                                                              \
        typename Tlhs,                                                      \
        typename Trhs,                                                      \
        typename = typename std::enable_if_t<std::is_scalar_v<Tlhs>>>       \
    inline BinaryMapExp<                                                    \
        OP_NAME<typename ScalarMapExp<Tlhs>::type, typename Trhs::type>,    \
        ScalarMapExp<Tlhs>, Trhs>                                           \
    operator OP_SYM (const Tlhs &lhs, const Exp<Trhs> &rhs) {               \
        static auto expr = ScalarMapExp<Tlhs>(lhs);                         \
        return F<OP_NAME>(expr, rhs);                                       \
    }

#define DECLARE_BINARY_OP_SYM_WITH_SCALAR_RIGHT(OP_NAME, OP_SYM)            \
    template <                                                              \
        typename Tlhs,                                                      \
        typename Trhs,                                                      \
        typename = typename std::enable_if_t<std::is_scalar_v<Trhs>>>       \
    inline BinaryMapExp<                                                    \
        OP_NAME<typename Tlhs::type, typename ScalarMapExp<Trhs>::type>,    \
        Tlhs, ScalarMapExp<Trhs> >                                          \
    operator OP_SYM (const Exp<Tlhs> &lhs, const Trhs &rhs) {               \
        static auto expr = ScalarMapExp<Trhs>(rhs);                         \
        return F<OP_NAME>(lhs, expr);                                       \
    }

/*
 * Unary Operator
 * - identity
 */
DECLARE_UNARY_OP(identity, a)

DECLARE_UNARY_OP(negation, -a)

/*
 * Binary Operator
 * - plus
 * - minus
 * - mul
 * - div
 * - mod
 */
DECLARE_BINARY_OP(plus, a + b)

DECLARE_BINARY_OP(minus, a - b)

DECLARE_BINARY_OP(mul, a * b)

DECLARE_BINARY_OP(div, a / b)

DECLARE_BINARY_OP(mod, a % b)

/*
 * Ternary Operator
 * - axpy
 */
DECLARE_TERNARY_OP(axpy, a * b + c)

/*
 * Wrapper for Binary Operator plus
 */
DECLARE_BINARY_OP_SYM(plus, +)

DECLARE_BINARY_OP_SYM_WITH_SCALAR_LEFT(plus, +)
DECLARE_BINARY_OP_SYM_WITH_SCALAR_RIGHT(plus, +)

/*
 * Wrapper for Binary Operator minus
 */
DECLARE_BINARY_OP_SYM(minus, -)

DECLARE_BINARY_OP_SYM_WITH_SCALAR_LEFT(minus, -)
DECLARE_BINARY_OP_SYM_WITH_SCALAR_RIGHT(minus, -)

/*
 * Wrapper for Binary Operator mul
 */
DECLARE_BINARY_OP_SYM(mul, *)

DECLARE_BINARY_OP_SYM_WITH_SCALAR_LEFT(mul, *)
DECLARE_BINARY_OP_SYM_WITH_SCALAR_RIGHT(mul, *)

/*
 * Wrapper for Binary Operator div
 */
DECLARE_BINARY_OP_SYM(div, /)

DECLARE_BINARY_OP_SYM_WITH_SCALAR_LEFT(div, /)
DECLARE_BINARY_OP_SYM_WITH_SCALAR_RIGHT(div, /)

/*
 * Wrapper for Binary Operator mod
 */
DECLARE_BINARY_OP_SYM(mod, %)

DECLARE_BINARY_OP_SYM_WITH_SCALAR_LEFT(mod, %)
DECLARE_BINARY_OP_SYM_WITH_SCALAR_RIGHT(mod, %)

} // namespace expr

} // namespace mou

#endif // MOU_EXPRESSION_H
