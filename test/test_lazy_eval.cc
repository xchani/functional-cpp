#include "../include/mou/lazy_eval.h"
#include <cstdio>
#include <type_traits>

using namespace mou;
using namespace mou::expr;

template <typename Tlhs, typename Trhs>
struct maximum {
    inline static auto Map(Tlhs a, Trhs b) {
        return a > b ? a : b;
    }
};

template <typename Tlhs, typename Trhs>
struct minimum {
    inline static auto Map(Tlhs a, Trhs b) {
        return a > b ? b : a;
    }
};

const int n = 3;

int main() {
    using DType = int;
    DType sa[n] = {1, 2, 3};
    DType sb[n] = {2, 3, 4};
    DType sc[n] = {3, 4, 5};
    DType sd[n] = {0, 0, 0};
    DType scalar = 3;
    Vec<DType> A(sa, n), B(sb, n), C(sc, n), D(sd, n);

    // test for scalar expression
    D = scalar * A;

    for (int i = 0; i < n; ++i) {
        printf("%d:%d == %d * %d\n", i, D.dptr[i], scalar, A.dptr[i]);
    }

    // test for unary and binary expression
    D = (F<identity>(B) / F<maximum>(C, B) + 2) % A;

    for (int i = 0; i < n; ++i) {
        printf("%d:%d == (id(%d) / max(%d, %d) + 2) %% %d\n",
               i, D.dptr[i], B.dptr[i], C.dptr[i], B.dptr[i], A.dptr[i]);
    }

    // test for ternary expression
    D = F<axpy>(A, B, C);

    for (int i = 0; i < n; ++i) {
        printf("%d:%d == %d * %d + %d\n",
               i, D.dptr[i], A.dptr[i], B.dptr[i], C.dptr[i]);
    }

    // test for scalar assign
    D = -1;

    for (int i = 0; i < n; ++i) {
        printf("%d:%d == %d\n", i, D.dptr[i], -1);
    }

    return 0;
}
