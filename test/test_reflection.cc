#include "../include/mou/reflection.h"
#include <cassert>

using namespace mou::reflection;

struct sc {
    int a;
    char b;
};

int main() {
    sc s { 1, 'a' };
    auto ts = tuple_binding(s);
    static_assert(std::is_same_v<decltype(ts), std::tuple<int, char>>);
    assert(std::get<0>(ts) == 1);
    assert(std::get<1>(ts) == 'a');

    return 0;
}
