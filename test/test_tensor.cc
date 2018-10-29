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
}

int main() {
    test_Shape();
    test_Tensor();

    return 0;
}