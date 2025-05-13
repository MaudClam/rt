
#ifndef SAMPLING_HPP
#define SAMPLING_HPP

#include "../../srcs/math/vec_utils.hpp"
#include "../../srcs/math/math_utils.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace sampling {

/// @brief Generates a uniform unit direction on a sphere with optional clamping or rejection of poles.
/// @tparam T Floating point type
/// @tparam Clamp Whether to use clamping instead of rejection
/// @param[out] dst Destination vector
/// @param[out] pole_hits Counts how often a polar zone was encountered
template <typename T, bool Clamp = true>
void safe_random_direction(vec<T,3>& dst, int& pole_hits) noexcept {
	constexpr T margin = math::constants<T>::eps * T(4);
	T z = static_cast<T>(math::random_double() * 2.0 - 1.0);

	if constexpr (Clamp) {
		if (T(1) - std::abs(z) < margin) {
			dst = { T(0), T(0), z < 0 ? T(-1) : T(1) };
			++pole_hits;
			return;
		}
	} else {
		while (T(1) - std::abs(z) < margin) {
			++pole_hits;
			z = static_cast<T>(math::random_double() * 2.0 - 1.0);
		}
	}

	const double phi = math::random_double() * math::constants<double>::two_pi;
	const T r = std::sqrt(std::max(T(0), T(1) - z * z));

	dst.set(r * std::cos(phi),
	        r * std::sin(phi),
	        z);
}

/// @brief Cosine-weighted direction sampling with clamp/reject protection against core zone.
/// @tparam T Floating point type
/// @tparam Clamp Whether to use clamping instead of rejection
/// @param[out] dst Output vector
/// @param[out] core_hits Counts how often a near-core zone was encountered
template <typename T, bool Clamp = true>
void safe_random_cosine_direction(vec<T,3>& dst, int& core_hits) noexcept {
	constexpr double min_r2 = 1e-5;
	double r2;

	if constexpr (Clamp) {
		r2 = std::lerp(min_r2, 1.0, math::random_double());
		++core_hits;
	} else {
		do {
			r2 = math::random_double();
		} while (r2 < min_r2);
		if (r2 < 2 * min_r2) ++core_hits;
	}

	const double r = std::sqrt(r2);
	const double z = std::sqrt(std::max(0.0, 1.0 - r2));
	const double phi = math::random_double() * math::constants<double>::two_pi;

	dst.set(static_cast<T>(r * std::cos(phi)),
	        static_cast<T>(r * std::sin(phi)),
	        static_cast<T>(z));
}

} // namespace sampling

#endif // SAMPLING_HPP
