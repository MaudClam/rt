#pragma once
#include <string>
#include <string_view>
#include <filesystem>
#include <type_traits>

namespace traits {

// base template: false by default
template<typename T>
struct is_string_like : std::false_type {};

// standard strings
template<> struct is_string_like<std::string> : std::true_type {};
template<> struct is_string_like<std::wstring> : std::true_type {};
template<> struct is_string_like<std::u8string> : std::true_type {};
template<> struct is_string_like<std::u16string> : std::true_type {};
template<> struct is_string_like<std::u32string> : std::true_type {};

// string_view
template<> struct is_string_like<std::string_view> : std::true_type {};
template<> struct is_string_like<std::wstring_view> : std::true_type {};
template<> struct is_string_like<std::u8string_view> : std::true_type {};
template<> struct is_string_like<std::u16string_view> : std::true_type {};
template<> struct is_string_like<std::u32string_view> : std::true_type {};

// C-style strings
template<> struct is_string_like<const char*> : std::true_type {};
template<> struct is_string_like<const wchar_t*> : std::true_type {};
template<> struct is_string_like<const char8_t*> : std::true_type {};
template<> struct is_string_like<const char16_t*> : std::true_type {};
template<> struct is_string_like<const char32_t*> : std::true_type {};

template<size_t N> struct is_string_like<char[N]> : std::true_type {};
template<size_t N> struct is_string_like<wchar_t[N]> : std::true_type {};
template<size_t N> struct is_string_like<char8_t[N]> : std::true_type {};
template<size_t N> struct is_string_like<char16_t[N]> : std::true_type {};
template<size_t N> struct is_string_like<char32_t[N]> : std::true_type {};

// std::filesystem::path
template<> struct is_string_like<std::filesystem::path> : std::true_type {};

// alias
template<typename T>
inline constexpr bool is_string_like_v = is_string_like<std::decay_t<T>>::value;

// Checking if a type can be inserted into an ostream
template<typename T>
using is_ostreamable = std::bool_constant<
	requires(std::ostream& os, T&& val) { os << std::forward<T>(val); }
>;

//template<typename T>
//constexpr void* stream_identity(T& stream) {
//	return static_cast<void*>(std::addressof(stream));
//}
//
//template<typename T>
//constexpr bool same_streams(const T& first) {
//	(void)first;
//	return true;
//}
//
//template<typename T, typename U, typename... Rest>
//constexpr bool same_streams(const T& first, const U& second, const Rest&... rest) {
//	return stream_identity(first) == stream_identity(second) && same_streams(second, rest...);
//}


} // namespace logging_utils
