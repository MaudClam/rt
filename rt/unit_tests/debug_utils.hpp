#ifndef DEBUG_UTILS_HPP
#define DEBUG_UTILS_HPP

#include <iostream>
#include <sstream>
#include <iomanip>
#include <locale>
#include <cassert>
#include "../srcs/math/math_utils.hpp"

#define PARAMETR_NAME_LENGTH	20
#define TEST_NAME_LENGTH		30
#define DISPLAYS_OF_PROGRESS	12

using namespace math;

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

/// @brief Same as above, but allows comparison with expected result
#define EVAL_FMT_(mod, expr, expr1) \
	do { \
		std::ostringstream os; \
		std::ostringstream os1; \
		auto __flags = std::cout.flags(); \
		auto __precision = std::cout.precision(); \
		os        << mod << (expr); \
		os1       << mod << (expr1); \
		std::cout << mod << #expr << ": " << os.str(); \
		if (os.str() != os1.str()) \
			std::cout << " ❌ expected: " << os1.str() << std::endl; \
		else \
			std::cout << std::endl; \
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

template <typename T>
std::string get_type_name() {
	if (std::is_same<T, float>::value) return "float";
	if (std::is_same<T, double>::value) return "double";
	if (std::is_same<T, long double>::value) return "long double";
	return "unknown type";
}

template <typename T>
void output_tests_parametrs(T attempts, T eps, T max) {
	char fill = ' ';
	std::cout << "TESTING PARAMETRS" << std::endl;
	std::cout << std::left << std::setw(PARAMETR_NAME_LENGTH) << std::setfill(fill);
	std::cout << "typename:" << get_type_name<T>() << std::endl;
	std::cout << std::left << std::setw(PARAMETR_NAME_LENGTH) << std::setfill(fill);
	std::cout << "number of attempts:" << attempts << std::endl;
	std::cout << std::left << std::setw(PARAMETR_NAME_LENGTH) << std::setfill(fill);
	std::cout << "permissible error:"  << eps << std::endl;
	std::cout << std::left << std::setw(PARAMETR_NAME_LENGTH) << std::setfill(fill);
	std::cout << "max coordinate:"  << max << std::endl;
}

void output_test_start_header(const std::string& str, bool invisible_test = true, int ext = 0) {
	static int name_length = TEST_NAME_LENGTH;
	if ( int(str.size()) + ext > name_length )
		name_length = int(str.size() + ext);
	std::cout << "▶️ " << std::left << std::setw(name_length) << std::setfill(' ') << str;
	if (invisible_test)
		std::cout << " started";
	else
		std::cout << std::endl;
}

template <typename T>
void output_visualization_commands(const vec<T,3>& v_inv, const vec<T,3>& n, const vec<T,3>& v) {
	std::cout << "COMMANDS FOR VISUALIZATION IN GEOGEBRA:\n";
	std::cout << "A = (0,0,0)\n";
	std::cout << "I = (" << v_inv << ")\n";
	std::cout << "N = (" << n << ")\n";
	std::cout << "R = (" << v << ")\n";
	std::cout << "Vector(I + A, A)\n";
	std::cout << "Vector(A, A + N)\n";
	std::cout << "Vector(A, A + R)\n";
}

template <typename T>
[[nodiscard]]
T random_around_num(T num, T epsilon) noexcept {
	return num + static_cast<T>(random_double() * 2.0) * epsilon - epsilon;
}

template <typename T>
[[nodiscard]]
bool is_range_exit(T val, T min, T max, T eps = constants<T>::eps) noexcept { // ∈ [min, max)
	if (val < min || val >= max || almost_equal<T>(val, max, eps))
		return true;
	return false;
}

template <typename T, typename F>
[[nodiscard]]
int count_circular_stability(T r, F reverse_transformation, int max_cycles = 1e+6) {
	using C = constants<T>;
	T current = r;
	for (int i = 1; i <= max_cycles; ++i) {
		current += C::two_pi;
		if (!almost_equal(reverse_transformation(current), r))
			return i - 1;
	}
	return max_cycles;
}

template <typename T, typename Func>
void iterate(T min, T step, T max, Func&& f, int param) {
	assert(step > T(0));
	while (min < max) {
		f(min, param);
		min += step;
	}
}

#endif // DEBUG_UTILS_HPP
