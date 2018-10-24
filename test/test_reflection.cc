#include <cassert>
#include "../include/reflection.h"

using namespace mou::reflection;

struct sc {
    int a;
    char b;
};

int main(void) {
    sc s;
    s.a = 1, s.b = 'a';
    auto ts = tuple_binding(s);
    static_assert(std::is_same_v<decltype(ts), std::tuple<int, char>>);
    assert(std::get<0>(ts) == 1);
    assert(std::get<1>(ts) == 'a');

    return 0;
}
