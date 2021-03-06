#include "../include/mou/logger.h"
#include "../include/mou/tensor.h"
#include <cstdio>
#include <iostream>
#include <type_traits>

using namespace mou;
using namespace mou::tensor;

void test_Shape() {
    Shape s1(2, 3, 4);
    CHECK_EQ(s1.Size(), 24);

    Shape s2;
    CHECK_EQ(s2.Size(), 0);

    Shape s3{2, 3, 4};
    CHECK_EQ(s3.Size(), 24);

    CHECK_EQ(s1[0], 2);
    CHECK_EQ(s1[1], 3);
    CHECK_EQ(s1[2], 4);

    CHECK_EQ(s1, s3);

    // assign with initializer_list will not change data type
    s3 = {3, 4, 5};
    CHECK_EQ(s3, Shape({3, 4, 5}));

    CHECK_EQ(s2.Dims(), 0);
    CHECK_EQ(s3.Dims(), 3);

    CHECK_EQ(s3.Size(0), 3);
    CHECK_EQ(s3.Size(1), 4);
    CHECK_EQ(s3.Size(2), 5);

    CHECK_EQ(s3.Size(0, 3), 60);
    CHECK_EQ(s3.Size(1, 3), 20);

    s3 = {};
    CHECK_EQ(s3.Dims(), 0);
}

void test_Tensor() {
    Shape s(2, 3);
    Tensor<int> a(s), b({1,2,3,4,5,6});

    CHECK_EQ(a.Shape_(), Shape(2, 3));
    CHECK_EQ(b.Shape_(), Shape(6));

    Tensor<int> b_copy(b);
    Tensor<int> b_move(std::move(b));
    Tensor<int> b_copy_asg(b_move);
    Tensor<int> b_move_asg(std::move(b_copy_asg));

    b = {2, 3, 4, 5, 6, 7};
    CHECK_EQ(b[2], 4);

    b.Reshape(s);
    CHECK_EQ(b.Shape_(), s);
    b.Reshape(Shape(6));
    CHECK_EQ(b.Shape_(), Shape(6));
    b.ReshapeLike(b_move);
    CHECK_EQ(b.Shape_(), b_move.Shape_());
    b.Reshape(1, 6, 1);
    CHECK_EQ(b.Shape_(), Shape(1, 6, 1));

    b[3] = -1;
    CHECK_EQ(b[3], -1);

    b = b * 3;
    CHECK_EQ(b[2], 12);

    Tensor<int> c(Shape(3));
    CHECK_EQ(c.Shape_(), Shape(3));
    c = b;
    CHECK_EQ(c.Shape_(), b.Shape_());

    c.Reshape(3, 2);
    CHECK_EQ(c.Slice(1, 3).Shape_(), Shape(2, 2));
    CHECK_EQ(c.SliceFrom(1).Shape_(), Shape(2, 2));
    CHECK_EQ(c.SliceTo(2).Shape_(), Shape(2, 2));

    int sum = 0;
    for (size_t i = 0; i < c.Size(); ++i) {
        sum += c[i] * c[i];
    }
    CHECK_EQ(Dot(c, c), sum);

    c = -1;
    CHECK_EQ(c[0], -1);
}

void test_Tensor_format() {
    Tensor<int> a(Shape(2, 3, 4, 5));
    for (size_t i = 0; i < a.Size(); ++i) {
        a[i] = i;
    }
    std::cout << a << std::endl;

    Tensor<int> b(Shape(1));
    b[0] = -1;
    std::cout << b << std::endl;

    Tensor<int> c(Shape(0));
    std::cout << c << std::endl;

    Tensor<float> d(Shape(2, 3));
    d = 0;
    d[0] = 1.123456789;
    d[2] = 123456789;
    std::cout << d << std::endl;

    Tensor<int> e(Shape(2, 2));
    e = -1;
    std::cout << e << std::endl;

    e = 0;
    std::cout << e << std::endl;
}

int main() {
    test_Shape();
    test_Tensor();
    test_Tensor_format();

    return 0;
}
