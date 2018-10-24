#include <variant>
#include <iostream>
#include <vector>
#include <string>
#include "../include/pattern_matching.h"

using namespace mou::pm;

int main(void) {
    using var_t = std::variant<int, long, double, std::string>;
    std::vector<var_t> vec = {10, 15l, 1.5, "hello"};
    for (auto& v: vec) {
        std::visit(overloaded {
            [](int arg) { std::cout << "int::" << arg << '\n'; },
            [](long arg) { std::cout << "long::" << arg << '\n'; },
            [](double arg) { std::cout << "double::" << arg << '\n'; },
            [](std::string arg) { std::cout << "string::" << arg << '\n'; },
        }, v);
    }
    return 0;
}
