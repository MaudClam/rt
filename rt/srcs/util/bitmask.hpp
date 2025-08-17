// bitmask.hpp
#pragma once
#include <type_traits>

namespace util {

template<class E> struct enable_bitmask : std::false_type {};
template<class E>
concept bitmask_enum = std::is_enum_v<E> && enable_bitmask<E>::value;

template<bitmask_enum E>
[[nodiscard]] constexpr E operator|(E a, E b) noexcept {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(a) | static_cast<U>(b));
}
template<bitmask_enum E>
[[nodiscard]] constexpr E operator&(E a, E b) noexcept {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(a) & static_cast<U>(b));
}
template<bitmask_enum E>
[[nodiscard]] constexpr E operator^(E a, E b) noexcept {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(a) ^ static_cast<U>(b));
}
template<bitmask_enum E>
[[nodiscard]] constexpr E operator~(E v) noexcept {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(~static_cast<U>(v));
}
template<bitmask_enum E>
constexpr E& operator|=(E& a, E b) noexcept { return a = a | b; }
template<bitmask_enum E>
constexpr E& operator&=(E& a, E b) noexcept { return a = a & b; }
template<bitmask_enum E>
constexpr E& operator^=(E& a, E b) noexcept { return a = a ^ b; }

template<bitmask_enum E>
[[nodiscard]] constexpr bool any(E v, E m) noexcept {
    using U = std::underlying_type_t<E>;
    return (static_cast<U>(v) & static_cast<U>(m)) != 0;
}
template<bitmask_enum E>
[[nodiscard]] constexpr bool all(E v, E m) noexcept {
    using U = std::underlying_type_t<E>;
    return (static_cast<U>(v) & static_cast<U>(m)) == static_cast<U>(m);
}
template<bitmask_enum E>
[[nodiscard]] constexpr bool none(E v) noexcept { return v == static_cast<E>(0); }

} // namespace util
