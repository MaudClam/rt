#pragma once
#include <ostream>
#include <type_traits>
#include <utility>
#include <functional> // std::invoke

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

// noexcept-aware std::ostream sequence
template<Ostreamable... Args>
inline std::ostream& write_sequence(std::ostream& os, Args&&... args)
noexcept(noexcept((os << ... << std::forward<Args>(args)))) {
    if constexpr (sizeof...(Args) > 0)
        (os << ... << std::forward<Args>(args));
    return os;
}

} // namespace traits
