#ifndef MATH_CONSTANTS_HPP
#define MATH_CONSTANTS_HPP


namespace math {

template <typename T>
struct constants {
	static constexpr long double pi_ld = 3.141592653589793238462643383279502884197L;
	static constexpr T pi			= static_cast<T>(pi_ld);
	static constexpr T two_pi		= static_cast<T>(2.0L * pi_ld);
	static constexpr T inv_pi		= static_cast<T>(1.0L / pi_ld);
	static constexpr T inv_two_pi	= static_cast<T>(0.5L / pi_ld);
	static constexpr T pi_half		= static_cast<T>(0.5L * pi_ld);
	static constexpr T deg2rad		= static_cast<T>(pi_ld / 180.0L);
	static constexpr T rad2deg		= static_cast<T>(180.0L / pi_ld);

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
