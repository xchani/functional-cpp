#include "../include/mou/pattern_matching.h"
#include <iostream>
#include <string>
#include <variant>
#include <vector>

using namespace mou::pm;

int main() {
    using var_t = std::variant<int, long, double, std::string, unsigned int>;
    std::vector<var_t> vec = {10, 15l, 1.5, "hello", 3u};
    for (auto& v: vec) {
        std::visit(overloaded {
            [](int arg) { std::cout << "int::" << arg << '\n'; },
            [](long arg) { std::cout << "long::" << arg << '\n'; },
            [](double arg) { std::cout << "double::" << arg << '\n'; },
            [](std::string arg) { std::cout << "string::" << arg << '\n'; },
            [](auto arg) { std::cout << "auto::" << arg << '\n'; },
        }, v);
    }
    return 0;
}
