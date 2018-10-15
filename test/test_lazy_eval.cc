#include <cstdio>
#include <type_traits>
#include "../include/lazy_eval.h"

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

int main(void) {
    using DType = int;
    DType sa[n] = {1, 2, 3};
    DType sb[n] = {2, 3, 4};
    DType sc[n] = {3, 4, 5};
    DType sd[n] = {0, 0, 0};
    Vec<DType> A(sa, n), B(sb, n), C(sc, n), D(sd, n);

    D = F<identity>(B) / F<maximum>(C, B);

    for (int i = 0; i < n; ++i) {
        printf("%d:%f == id(%f) / max(%f, %f)\n",
                i,
                static_cast<float>(D.dptr[i]),
                static_cast<float>(B.dptr[i]),
                static_cast<float>(C.dptr[i]),
                static_cast<float>(B.dptr[i]));
    }

    D = F<axpy>(A, B, C);

    for (int i = 0; i < n; ++i) {
        printf("%d:%f == %f * %f + %f) \n",
                i,
                static_cast<float>(D.dptr[i]),
                static_cast<float>(A.dptr[i]),
                static_cast<float>(B.dptr[i]),
                static_cast<float>(C.dptr[i]));
    }

    return 0;
}
