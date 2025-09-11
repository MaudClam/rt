#pragma once
#include <iosfwd>
#include <string_view>
#include <type_traits>
#include <utility>
#include <functional>

namespace traits {

// Concept: T is streamable to std::ostream
template<typename T>
concept Ostreamable = requires(std::ostream& os, T&& t) {
    { os << std::forward<T>(t) } -> std::same_as<std::ostream&>;
};

// Concept: F is writer who can write to std::ostream
template<typename F, typename... Args>
concept OsWriter = requires(F f, std::ostream& os, Args&&... args) {
    std::invoke(std::forward<F>(f), os, std::forward<Args>(args)...);
};

// Concept: There is a free function as_sv(x) → std::string_view
template<class T>
concept AdlAsSv = requires(const T& x) {
    { as_sv(x) } -> std::convertible_to<std::string_view>;
};

} // namespace traits
