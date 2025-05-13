#ifndef MATH_CONSTANTS_HPP
#define MATH_CONSTANTS_HPP

#include <random>


namespace math {

template <typename T>
struct constants {
	static constexpr T pi			= T(3.14159265358979323846);
	static constexpr T two_pi		= T(2) * pi;
	static constexpr T deg2rad		= pi / T(180);
	static constexpr T rad2deg		= T(180) / pi;
	static constexpr T eps			= std::numeric_limits<T>::epsilon();
	static constexpr T polar_zone	= std::is_same_v<T, float> ? T(1e-5) : T(1e-10);

//	Constants for color and accuracy
	static constexpr T inv_255 = T(1) / T(255);
	static constexpr T inv_256 = T(1) / T(256);
	static constexpr T almost_1_255 = T(1) - inv_255;
	static constexpr T almost_1_256 = T(1) - inv_256;
};


} // namespace math

#endif // MATH_CONSTANTS_HPP
