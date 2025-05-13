#ifndef DEBUG_UTILS_HPP
#define DEBUG_UTILS_HPP

#include <iostream>
#include <sstream>
#include <iomanip>
#include <locale>


/// @brief Outputs an expression and its result: expr → "expr: value"
/// Uses std::cout by default.
#define EVAL(expr) \
	do { \
		std::cout << #expr << ": " << (expr) << std::endl; \
	} while (false)

/// @brief Same as above, but allows you to specify a stream (e.g. std::cerr, or a modified std::cout).
#define EVAL_TO(out, expr) \
	do { \
		(out) << #expr << ": " << (expr) << std::endl; \
	} while (false)

/// @brief The same, but allows you to specify output modifiers (std::scientific, etc.).
/// Applies to both parts of the output: the expression and the result.
#define EVAL_FMT(mod, expr) \
	do { \
		auto __flags = std::cout.flags(); \
		auto __precision = std::cout.precision(); \
		std::cout << mod << #expr << ": " << (expr) << std::endl; \
		std::cout.flags(__flags); \
		std::cout.precision(__precision); \
	} while (false)

struct comma_separator : std::numpunct<char> {
protected:
	char do_thousands_sep() const override { return '\''; } // разделитель
	std::string do_grouping() const override { return "\3"; } // группы по 3
};

template <typename T, size_t N> struct vec;

template <typename T>
[[nodiscard]]
constexpr T generate_nan() {
	return T(0) / T(0);
}

template <typename T, size_t N>
constexpr vec<T,N> generate_nan_vec() {
	vec<T,N> v{};
	if constexpr (N > 0) v[0] = T(0) / T(0); // guaranteed NaN
	if constexpr (N > 1) v[1] = T(1);
	if constexpr (N > 2) v[2] = T(2);
	return v;
}

template <typename T>
std::string with_separator(T value) {
	std::ostringstream os;
	os.imbue(std::locale(std::cout.getloc(), new comma_separator));
	os << value;
	return os.str();
}

#endif // DEBUG_UTILS_HPP
