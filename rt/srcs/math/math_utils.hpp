#ifndef MATH_UTILS
#define MATH_UTILS

#include <cmath>              // std::abs, std::sin, std::cos, etc.
#include <limits>             // std::numeric_limits
#include <random>
#include "math_constants.hpp"


namespace math {

// Rounding to the nearest multiple of eps
// For float/double — works fully
// For integral types — returns as is
template <typename T>
[[nodiscard]]
constexpr T round_to_eps(T x, T eps = math::constants<T>::eps) noexcept {
	if constexpr (std::is_integral_v<T>) {
		return x; // don't touch whole types
	} else {
		return std::round(x / eps) * eps;
	}
}

//	Removes "-0"
template <typename T>
[[nodiscard]]
constexpr T clean_zero(T x) noexcept {
	return (x == T(0) && std::signbit(x)) ? T(0) : x;
}

// Ready for output to ostream
// Suppresses "-0", rounds float/double by eps, ignores integral types
template <typename T>
[[nodiscard]]
constexpr T safe_format(T x) noexcept {
	return clean_zero(round_to_eps(x));
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

/// @brief Normalizes an angle to the range [0, 2π).
/// @details This function maps any input angle into the canonical positive range
/// [0, 2π), which is often used in applications where negative angles are not meaningful,
/// such as texture coordinates, azimuth angles, or polar coordinate systems.
///
/// Mathematically:
///     normalize_angle(φ) = φ mod 2π
///     If φ < 0 → add 2π to ensure non-negative result.
///
/// @tparam T A floating-point type (float, double, etc.)
/// @param phi Angle in radians (can be negative or greater than 2π).
/// @return Normalized angle in the range [0, 2π).
template <typename T>
[[nodiscard]]
constexpr T normalize_angle(T phi) noexcept {
	phi = std::fmod(phi, constants<T>::two_pi);
	phi = phi < T(0) ? phi + constants<T>::two_pi : phi;
	return phi;
}

/// @brief Wraps an angle to the range (-π, π].
/// @details This function normalizes any angle to the principal range (-π, π].
/// This is useful to ensure angle comparisons, interpolation, and periodic logic
/// behave correctly and avoid discontinuities across ±π.
///
/// Mathematically:
///     wrap_angle(θ) = ((θ + π) mod 2π) - π
///
/// @tparam T A floating-point type (float, double, etc.)
/// @param angle The input angle in radians.
/// @return Equivalent angle in the range (-π, π].
template <typename T>
[[nodiscard]]
constexpr T wrap_angle(T angle) noexcept {
	return std::fmod(angle + constants<T>::pi, constants<T>::two_pi) - constants<T>::pi;
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

} // namespace math

#endif // MATH_UTILS_HPP
