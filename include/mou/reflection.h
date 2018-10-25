#ifndef MOU_REFLECTION_H
#define MOU_REFLECTION_H

#include <tuple>
#include <type_traits>

namespace mou {

namespace reflection {

/*
 * Example:
 *  struct sc { int a; char b; };
 *  sc s; s.a = 1; s.b = 'a';
 *  auto ts = tuple_binding(s);
 *  static_assert(std::is_same_v<decltype(ts), std::tuple<int, char>>);
 *  assert(std::get<0>(ts) == 1);
 *  assert(std::get<1>(ts) == 'a');
 */

struct any_type {
    template <typename T>
    constexpr operator T() const;
};

template <typename T, typename... Any>
decltype(void(T{std::declval<Any>()...}), std::true_type{})
_is_constructible(int);

template <typename T, typename... Any>
std::false_type
_is_constructible(...);

template <typename T, typename... Any>
using is_constructible = decltype(_is_constructible<T, Any...>(0));

template <typename T>
auto tuple_binding(T&& t) noexcept {
    using type = std::decay_t<T>;
    if constexpr (is_constructible<type, any_type, any_type, any_type>::value) {
        auto&& [a, b, c] = std::forward<T&&>(t);
        return std::make_tuple(a, b, c);
    } else if constexpr (is_constructible<type, any_type, any_type>::value) {
        auto&& [a, b] = std::forward<T&&>(t);
        return std::make_tuple(a, b);
    } else if constexpr (is_constructible<type, any_type>::value) {
        auto&& [a] = std::forward<T&&>(t);
        return std::make_tuple(a);
    } else {
        return std::make_tuple();
    }
}

}; // namespace reflection

}; // namespace mou

#endif // MOU_REFLECTION_H
