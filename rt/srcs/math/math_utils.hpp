#ifndef MATH_UTILS
#define MATH_UTILS

#include <cmath>              // std::abs, std::sin, std::cos, etc.
#include <limits>             // std::numeric_limits
#include <random>			  // std::mt19937, etc.
#include <sstream>            // for operator>>
#include <cassert>            // for assert
#include <type_traits>        // std::is_floating_point_v
#include "math_constants.hpp"


namespace math {

/// @brief Rounding to the nearest multiple of eps.
/// For float/double — works fully.
/// For integral types — returns as is.
template <typename T>
[[nodiscard]]
constexpr T round_to_eps(T x, T eps = constants<T>::eps) noexcept {
	if constexpr (std::is_integral_v<T>) {
		return x; // don't touch whole types
	} else {
		return std::round(x / eps) * eps;
	}
}

/// @brief Removes "-0".
template <typename T>
[[nodiscard]]
constexpr T clean_zero(T x) noexcept {
	return (x == T(0) && std::signbit(x)) ? T(0) : x;
}

/// @brief Ready for output to ostream.
/// Suppresses "-0", rounds float/double by eps, ignores integral types.
template <typename T>
[[nodiscard]]
constexpr T safe_format(T x, T eps = constants<T>::eps) noexcept {
	return clean_zero(round_to_eps(x, eps));
}

template <typename T>
[[nodiscard]]
constexpr T de_nan(T x) noexcept {
	return x != x ? T(0) : x;
}

template <typename T>
[[nodiscard]]
constexpr T safe_division(T x, T y, T eps = constants<T>::eps) noexcept {
	return x / (std::abs(y) < eps ? std::copysign(eps, y) : y);
}

template <typename T>
[[nodiscard]]
constexpr T clamp(T x, T lo, T hi) noexcept {
	return std::min(hi, std::max(lo, x));
}

/// @brief Compares two floating-point values for approximate equality.
/// @tparam T Must be a floating-point type.
template <typename T>
[[nodiscard]]
constexpr bool almost_equal(T a, T b, T eps = constants<T>::eps) noexcept {
	static_assert(std::is_floating_point_v<T>, "almost_equal<T>: T must be floating-point");
	return std::abs(a - b) < eps * T(10);
}

/// @brief Normalize a value into the range [0, unit).
/// @details Maps any real value x to the range [0, unit), useful for angles, texture coords, etc.
/// @tparam T Floating-point type.
/// @param x Input value.
/// @param unit Period (e.g., 1 for unit interval, 2π for angles).
/// @return Normalized value in [0, unit).
template <typename T>
[[nodiscard]]
constexpr T normalize(T x, T unit, T inv_unit) noexcept {
	return x - std::floor(x * inv_unit) * unit;
}
template <typename T>
[[nodiscard]]
constexpr T normalize(T x, T unit = T(1)) noexcept {
	return normalize(x, unit, T(1) / unit);
}
template <typename T>
[[nodiscard]]
constexpr T normalize_angle(T angle) noexcept {
	using C = constants<T>;
	return normalize(angle, C::two_pi, C::inv_two_pi);
}

/// @brief Wraps a value into the range [-unit/2, unit/2).
/// @details Useful for wrapping symmetric periodic values (like angles).
/// @tparam T Floating-point type.
/// @param x Input value.
/// @param unit Period (e.g., 1 for unit wrap, 2π for angle wrap).
/// @return Wrapped value in [-unit/2, unit/2).
template <typename T>
[[nodiscard]]
constexpr T wrap(T x, T unit, T unit_half, T inv_unit) noexcept {
	return x - std::floor((x + unit_half) * inv_unit) * unit;
}
template <typename T>
[[nodiscard]]
constexpr T wrap(T x, T unit = T(1)) noexcept {
	return wrap(x, unit, T(0.5) * unit, T(1) / unit);
}
template <typename T>
[[nodiscard]]
constexpr T wrap_angle(T x) noexcept {
	using C = constants<T>;
	return wrap(x, C::two_pi, C::pi, C::inv_two_pi);
}

/// @brief Compares two angles for approximate equality with adaptive precision.
/// @details This function compares two angles `a` and `b`, considering circular
/// periodicity and accumulated floating-point errors from full rotations.
///
/// It normalizes both angles into the [0, 2π) range and scales the epsilon tolerance
/// proportionally to the number of full rotations involved (based on multiples of π),
/// ensuring robust comparisons even for large angles like `10000 * π`.
///
/// This is particularly important in applications involving trigonometric wrapping,
/// animation cycles, or long-term angular accumulation.
///
/// @warning When comparing very small angles like π/360 (≈ 0.0087), large values of `a`
/// may alias to `b` due to limited float precision (e.g. `10000 * π + π/360 == π`).
/// Use with care in precision-critical comparisons.
///
/// @tparam T Must be a floating-point type (e.g. float, double).
/// @param a First angle in radians.
/// @param b Second angle in radians.
/// @param eps Base angular tolerance, default is `constants<T>::eps`
/// @return true if angles are approximately equal.
template <typename T>
[[nodiscard]]
constexpr bool almost_equal_angles(T a, T b, T eps = constants<T>::eps) noexcept {
	static_assert(std::is_floating_point_v<T>, "almost_equal_angles<T>: T must be floating-point");
	int turns_a = std::abs(static_cast<int>(std::round(a / constants<T>::pi)));
	int turns_b = std::abs(static_cast<int>(std::round(b / constants<T>::pi)));
	T adaptive_eps = eps * (T(1) + std::max(turns_a, turns_b));
	a = normalize_angle(a);
	b = normalize_angle(b);
	return almost_equal(a, b, adaptive_eps);
}

/// @brief returns the true if the angle a is less than the angle b (in the circular sense), which means,
/// if b is “ahead” a, in the direction counterclockwise, on the arc < π.
/// @see wrap_angle()
template <typename T>
[[nodiscard]]
constexpr bool compare_angles(T a, T b, T eps = constants<T>::eps) noexcept {
	using C = constants<T>;
	T d = wrap_angle(a - b);
	return d < T(0) && std::abs(d) < C::pi - eps;
}

template <typename T>
[[nodiscard]]
constexpr T to_radians(T degrees) noexcept {
	return degrees * constants<T>::deg2rad;
}

template <typename T>
[[nodiscard]]
constexpr T to_degrees(T radians) noexcept {
	return radians * constants<T>::rad2deg;
}

template <typename T>
[[nodiscard]]
constexpr T spherical_phi(T y, T x) noexcept {
	return normalize_angle(std::atan2(y, x));
}

template <typename T>
[[nodiscard]]
constexpr T spherical_theta(T z, T len = T(1)) noexcept {
	return std::acos(clamp(safe_division(z, len), T(-1), T(1)));
}

//	Random number in range [0.0, 1.0)
inline double random_double() noexcept {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<> dis(0.0, 1.0);
	return dis(gen);
}

bool remove_separator_if(std::istringstream& is, std::string sep) {
	auto pos = is.tellg();
	std::string trash;
	is >> trash;
	if (trash == sep)
		return true;
	else
		is.seekg(pos);
	return false;
}

} // namespace math

#endif // MATH_UTILS_HPP
