#pragma once
#include <type_traits>     // std::is_convertible, std::decay_t
#include <string>          // std::string
#include <string_view>     // std::string_view и wide_view
#include <ostream>         // std::ostream
#include <utility>         // std::declval (in a more general form)
#include <filesystem>      // std::filesystem::path

namespace traits {

// Concept: T is streamable to std::ostream
template<typename T>
concept Ostreamable = requires(std::ostream& os, T&& t) {
    { os << std::forward<T>(t) } -> std::same_as<std::ostream&>;
};

// noexcept-aware ostream sequence
template<Ostreamable... Args>
inline std::ostream& sequence(std::ostream& os, const Args&... args)
    noexcept((noexcept(os << args) && ...)) {
    return (os << ... << args);
}

// Trait: whether streaming T is noexcept via sequence()
template<typename T>
inline constexpr bool write_noexcept =
    noexcept(sequence(std::declval<std::ostream&>(), std::declval<T>()));

// Narrow set: std::string_view, const char*, string literals
template<typename T>
concept StringLike =
    std::is_convertible_v<T, std::string_view>;

// Wide range: support for wchar_t, UTF-16, UTF-32, + filesystem::path
template<typename T>
concept WStringLike =
    std::is_convertible_v<T, std::wstring_view>   ||
    std::is_convertible_v<T, std::u16string_view> ||
    std::is_convertible_v<T, std::u32string_view> ||
    std::is_same_v<std::decay_t<T>, std::filesystem::path>;

template<typename T>
concept AnyStringLike = StringLike<T> || WStringLike<T>;

template<typename T>
concept OutputStream = requires(T os) {
    { os << std::declval<std::string_view>() } -> std::same_as<decltype(os)&>;
};

template<typename T>
inline constexpr bool is_any_string_like_v = StringLike<T> || WStringLike<T>;

} // namespace traits
