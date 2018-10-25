#ifndef MOU_PATTERN_MATCHING_H
#define MOU_PATTERN_MATCHING_H

namespace mou {

namespace pm {

template <typename... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

}; // namespace pm

}; // namespace mou

#endif // MOU_PATTERN_MATCHING_H
