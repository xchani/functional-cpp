#include "../include/mou/tensor.h"
#include <cstdio>
#include <iostream>
#include <type_traits>

using namespace mou;
using namespace mou::tensor;

int main() {
    using DType = int;
    Shape s1(2, 3), s2(2, 3);
    s2 = {3, 2};
    Tensor<DType> A(s1), B(s2), C{1, 2, 3, 4, 5, 6};

    std::cout << "C shape: " << C.Shape_() << std::endl;

    C.Reshape(Shape(2, 3));
    A = C;
    C.Reshape(Shape(3, 2));
    B = C;

    std::cout << "A shape: " << A.Shape_() << std::endl;
    std::cout << "A data: " << A << std::endl;
    std::cout << "B shape: " << B.Shape_() << std::endl;
    std::cout << "B data: " << B << std::endl;
    std::cout << "C shape: " << C.Shape_() << std::endl;
    std::cout << "C data: " << C << std::endl;

    return 0;
}
