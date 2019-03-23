## Mou
Library for C++ 17 containing funtional features and so on.

### Examples

#### Tensor
```C++
using namespace mou::tensor;
using namespace mou::expr;

Tensor<int> x({1, 2, 3, 4});
std::cout << x << std::endl;

x.Reshape(2, 2);
std::cout << x << std::endl;

x = x * 3;
std::cout << x << std::endl;

x = F<axpy>(x, x, x);
std::cout << x << std::endl;

x = -1;
std::cout << x << std::endl;
```

Output:
```C++
[ 1, 2, 3, 4]
<NDArray 4 @cpu(0)>

[[ 1, 2],
 [ 3, 4]]
<NDArray 2x2 @cpu(0)>

[[  3,  6],
 [  9, 12]]
<NDArray 2x2 @cpu(0)>

[[  12,  42],
 [  90, 156]]
<NDArray 2x2 @cpu(0)>

[[ -1, -1],
 [ -1, -1]]
<NDArray 2x2 @cpu(0)>
```

#### Pattern Matching
```C++
using namespace mou::pm;

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
```

Output:
```C++
int::10
long::15
double::1.5
string::hello
auto:3
```

#### Reflection
```C++
using namespace mou::reflection;

struct sc {
    int a;
    char b;
};

sc s{ 1, 'a' };
auto ts = tuple_binding(s);
static_assert(std::is_same_v<decltype(ts), std::tuple<int, char>>);
std::cout << std::get<0>(ts) << std::endl;
std::cout << std::get<1>(ts) << std::endl;
```

Output:
```C++
1
a
```

### Features
- [x] Lazy Evaluation
    - [x] Scalar Expression
    - [x] Unary Expression for Vector
    - [x] Binary Expression for Vector
    - [x] Ternary Expression for Vector
    - [x] Deduce return type of expression
- [ ] Pattern Matching
    - [x] Type-matching with overloaded operator()'s
- [ ] Reflection
    - [x] Simple static reflection
- [ ] Logger
    - [x] Checker
    - [ ] Logger
- [ ] Tensor
    - [x] Basic Tensor
    - [x] Slicing
    - [x] Dot Product
    - [ ] GEMM
    - [ ] SIMD using instrinsics for Dot Product
    - [ ] SIMD using instrinsics for GEMM
    - [ ] CUDA kernel for Dot Product
    - [ ] CUDA kernel for GEMM
