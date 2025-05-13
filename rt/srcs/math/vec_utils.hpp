#ifndef VEC_UTILS_HPP
#define VEC_UTILS_HPP

#include <utility>            // std::forward
#include <functional>         // std::plus, std::multiplies
#include <random>
#include "vec.hpp"            // vec<T,N>
#include "onb.hpp"            // vec<T,N>
#include "math_utils.hpp"     // clamp, safe_division...


template <typename T, size_t N, typename F>
constexpr vec<T,N>	map(const vec<T,N>& v, F&& f) noexcept(noexcept(f(v[0]))) {
	vec result = v;
	return result.map(std::forward<F>(f));
}

template <typename T, size_t N, typename F>
constexpr vec<T,N>	zip_with(const vec<T,N>& a, const vec<T,N>& b, F&& f) noexcept(noexcept(f(a[0], b[0]))) {
	vec result = a;
	return result.zip_with(b, std::forward<F>(f));
}

template <typename T, size_t N>
[[nodiscard]]
constexpr T dot(const vec<T,N>& a, const vec<T,N>& b) noexcept {
	T sum = T(0);
	for (size_t i = 0; i < N; ++i)
		sum += a[i] * b[i];
	return sum;
}

template <typename T, size_t N>
[[nodiscard]]
constexpr vec<T,N>	operator+(const vec<T,N>& a, const vec<T,N>& b) noexcept {
	return zip_with(a, b, [](T x, T y) { return x + y; });
}

template <typename T, size_t N>
[[nodiscard]]
constexpr vec<T,N>	operator-(const vec<T,N>& a, const vec<T,N>& b) noexcept {
	return zip_with(a, b, [](T x, T y) { return x - y; });
}

template <typename T, size_t N>
[[nodiscard]]
constexpr vec<T,N>	operator*(const vec<T,N>& v, T s) noexcept {
	return map(v, [s](T x) { return x * s; });
}

template <typename T, size_t N>
[[nodiscard]]
constexpr vec<T,N>	operator*(T s, const vec<T,N>& v) noexcept {
	return map(v, [s](T x) { return x * s; });
}

template <typename T, size_t N>
[[nodiscard]]
constexpr T			operator*(const vec<T,N>& a, const vec<T,N>& b) noexcept {
	return dot(a, b);
}

template <typename T, size_t N>
[[nodiscard]]
constexpr vec<T,N>	operator/(const vec<T,N>& v, T s) {
	return map(v, [s](T x) { return math::safe_division(x, s); });
}

template <typename T>
[[nodiscard]]
constexpr vec<T,3>	operator^(const vec<T,3>& a, const vec<T,3>& b) noexcept {
	return {
		a[1]*b[2] - a[2]*b[1],
		a[2]*b[0] - a[0]*b[2],
		a[0]*b[1] - a[1]*b[0]
	};
}

template <typename T>
[[nodiscard]]
constexpr vec<T,3>	cross(const vec<T,3>& a, const vec<T,3>& b) noexcept {
	return  a ^ b;
}

template <typename T, size_t N>
[[nodiscard]]
constexpr bool		operator==(const vec<T,N>& a, const vec<T, N>& b) noexcept {
	for (size_t i = 0; i < N; ++i)
		if (a[i] != b[i]) return false;
	return true;
}

template <typename T, size_t N>
[[nodiscard]]
constexpr bool		operator!=(const vec<T,N>& a, const vec<T,N>& b) noexcept {
	return !(a == b);
}

/// @brief Converts spherical coordinates phi, theta to Cartesian coordinates of a unit vector (x, y, z).
/// @tparam T Floating point type (float or double)
/// @param phi spherical coordinate.
/// @param theta spherical coordinate.
/// @param[out] dst Reference to the destination vector.
template <typename T>
void from_spherical_unit(T phi, T theta, vec<T,3>& dst) noexcept {
	const T sin_theta = std::sin(theta);
	const T cos_theta = std::cos(theta);
	const T cos_phi   = std::cos(phi);
	const T sin_phi   = std::sin(phi);
	dst.set(sin_theta * cos_phi,
			sin_theta * sin_phi,
			cos_theta);
}

/// @brief Converts Cartesian coordinates (x, y, z) to spherical (phi, theta, r).
/// @param src Reference to the source vector.
/// @param[out] dst Reference to the destination vector.
/// @note  If the length of the source vector is less than epsilon, all values ​​of the target vector are set to zero.
template <typename T>
void to_spherical(const vec<T,3>& src, vec<T,3>& dst) noexcept {
	const T len = src.length();
	dst.set(math::spherical_phi(src.y(), src.x()),
			math::spherical_theta(src.z(), len),
			len);
}

/// @brief Converts spherical coordinates (phi, theta, r) ​​to Cartesian coordinates (x, y, z).
/// @tparam T Floating point type (float or double)
/// @param src Reference to the source vector.
/// @param[out] dst Reference to the destination vector.
/// @note  If r in the source vector is less than epsilon, all values ​​of the target vector are set to zero.
template <typename T>
void from_spherical(const vec<T,3>& src, vec<T,3>& dst) noexcept {
	const T r_val = src.r();
	if (r_val > math::constants<T>::eps) {
		from_spherical_unit(src.phi(), src.theta(), dst);
		dst *= r_val;
	} else {
		dst.fill(T(0));
	}
}

/// @brief Generates a uniformly distributed unit direction vector on a sphere.
/// @details This function samples a random point on the unit sphere using spherical coordinates.
/// The azimuthal angle φ is chosen uniformly in [0, 2π), and z ∈ [−1, 1). The x and y components are
/// reconstructed from φ and z to ensure a unit vector. If the generated z is too close to ±1 (near the poles),
/// the direction is clamped to (0, 0, ±1) to avoid precision issues near r = sqrt(1 - z²) → 0.
/// All calculations involving φ are performed in double precision to maintain angular accuracy.
/// @tparam T Floating point type (float or double)
/// @param[out] dst Output normalized direction vector
template <typename T>
void random_direction(vec<T,3>& dst) noexcept {
	T z = static_cast<T>(math::random_double() * 2.0 - 1.0);
	if (T(1) - std::abs(z) < math::constants<T>::polar_zone) {
		dst.set(T(0), T(0), z < 0 ? T(-1) : T(1));
		return;
	}
	const double phi = math::random_double() * math::constants<double>::two_pi;
	const T r = std::sqrt(std::max(T(0), T(1) - z * z));
	dst.set(r * std::cos(phi), r * std::sin(phi), z);
}

/// @brief Generates a cosine-weighted random unit vector over the upper hemisphere.
/// @details This function samples directions over the hemisphere oriented along the +z axis
/// using cosine-weighted distribution. It generates a squared radial component r² uniformly in [0, 1),
/// and maps it into Cartesian coordinates using polar coordinates with an azimuthal angle φ ∈ [0, 2π).
/// To avoid unstable directions near the north pole (r ≈ 0), values of r² below
/// `math::constants<T>::polar_zone` are clamped. All intermediate calculations are
/// performed in double precision where necessary to reduce round-off errors.
/// @tparam T Floating-point type (e.g., float or double).
/// @param[out] dst Reference to the resulting normalized direction vector.
template <typename T>
void random_cosine_direction(vec<T,3>& dst) noexcept {
	T r2 = static_cast<T>(math::random_double());
	if (r2 < math::constants<T>::polar_zone) {
		r2 = math::constants<T>::polar_zone;
	}
	const T r = std::sqrt(r2);
	const T z = std::sqrt(std::max(T(0), T(1) - r2));
	const double phi = math::random_double() * math::constants<double>::two_pi;
	dst.set(static_cast<T>(r * std::cos(phi)),
			static_cast<T>(r * std::sin(phi)),
			static_cast<T>(z));
}

template <typename T>
void rotate(T& x, T& y, T angle) noexcept {
	if (angle != T(0)) {
		T sin = std::sin(angle), cos = std::cos(angle);
		T x_ = x * cos - y * sin;
		T y_ = x * sin + y * cos;
		x = x_; y = y_;
	}
}

/// @brief Rotate this vector around a normalized axis by the given angle (in radians).
/// @tparam T Floating point type (float or double)
/// @param axis Reference to the normalized axis vector (must have unit length).
/// @param angle The rotation angle in radians.
/// @param[out] dst Reference to the destination vector.
template <typename T>
void rotate_around_axis(const vec<T,3>& axis, T angle, vec<T,3>& dst) noexcept {
	rotate_around_axis<T>(axis, std::sin(angle), std::cos(angle), dst);
}
template <typename T>
void rotate_around_axis(const vec<T,3>& axis, T sin, T cos, vec<T,3>& dst) noexcept {
	vec<T,3> tmp[2];
	tmp[0].cross(axis, dst) *= sin;
	tmp[1].set(axis) *= (dot(axis, dst) * (T(1) - cos));
	dst *= cos;
	dst += tmp[0];
	dst	+= tmp[1];
}

/// @brief Reflects a vector around the given surface normal.
/// @tparam T Floating point type (float or double)
/// @param norm Reference to the normalized surface normal.
/// @param[out] refl Reference to the input (destination) vector to reflect.
/// @pre   Both the input vector and the normal are expected to be normalized.
template <typename T>
void reflect_direction(const vec<T,3>& norm, vec<T,3>& refl) noexcept {
	refl -= norm * (dot(refl, norm) * T(2));
	refl.normalize();
}

/// @brief Refracts the vector using eta and returns Schlick reflectance coefficient.
/// @tparam T Floating point type (float or double)
/// @param normal Unit surface normal (oriented).
/// @param eta Ratio of refractive indices. Already adjusted for ray direction.
/// @param[out] refr Reference to the input (destination) vector to refracts.
/// @param threshold If reflectance R ≥ threshold, refraction is skipped. Default ≈ 1 - 1/256.
/// @return Schlick reflectance R ∈ [0, 1]. Returns 1.0 on total internal reflection or deliberate skip.
/// @pre  Both the input vector and the normal are expected to be normalized.
/// @note In cases where the Schlick reflectivity R == 1, the input (target) vector is not modified.
template <typename T>
[[nodiscard]]
constexpr T refract_schlick_direction(const vec<T,3>& normal, T eta, vec<T,3>& refr,
		T threshold = math::constants<T>::almost_1_256) noexcept {
	const T cos_theta = -(refr * normal);
	const T sin2_theta = T(1) - cos_theta * cos_theta;
	const T k = T(1) - eta * eta * sin2_theta;
	if (k < T(0)) {
		return T(1); // total internal reflection
	}
	const T r0 = std::pow((T(1) - eta) / (T(1) + eta), 2);
	const T R = r0 + (T(1) - r0) * std::pow(T(1) - cos_theta, 5);
	if (R >= threshold)
		return T(1); // treat as reflection
	refr *= eta;
	refr += normal * (eta * cos_theta - std::sqrt(k));
	refr.normalize();
	return R;
}

/// @brief Convert Cartesian coordinates to screen coordinates.
/// The center of the Cartesian system corresponds to the center of the screen.
/// The Y axis is inverted (down on the screen - up in Cartesian).
/// @tparam T Floating point type (float or double)
/// @param width Screen width.
/// @param height Screen height.
/// @param[out] dst Reference to the input (destination) vector to convert.
template <typename T, size_t N>
void to_screen(T width, T height, vec<T,N>& dst) noexcept requires (N >= 2) {
	dst.x() += width / T(2);
	dst.y() = height / T(2) - dst.y();
}

/// @brief Convert screen coordinates to Cartesian.
/// The center of the screen becomes the point (0,0).
/// The Y axis is inverted (up in Cartesian - down on the screen).
/// @tparam T Floating point type (float or double)
/// @param width Screen width.
/// @param height Screen height.
/// @param[out] dst Reference to the input (destination) vector to convert.
template <typename T, size_t N>
void from_screen(T width, T height, vec<T,N>& dst) noexcept requires (N >= 2) {
	dst.x() -= width / T(2);
	dst.y() = T(2) * (height / T(2)) - dst.y();
}

/// @brief Converts a linear index to screen coordinates.
/// @tparam T Floating point type (float or double)
/// @param width The width of the image or buffer.
/// @param index Linear index.
/// @param[out] dst Reference to the input (destination) vector to convert.
template <typename T, size_t N>
void line_to_screen(size_t width, size_t index, vec<T,N>& dst) noexcept requires (N >= 2) {
	dst.u() = static_cast<T>(index % width);
	dst.v() = static_cast<T>(index / width);
}

/// @brief Converts screen coordinates to a linear index.
/// @tparam T Floating point type (float or double)
/// @param width The width of the image or buffer.
/// @param[out] dst Reference to the input (destination) vector to convert.
/// @return Linear index.
template <typename T, size_t N>
[[nodiscard]]
constexpr size_t screen_to_line(size_t width, vec<T,N>& dst) noexcept requires (N == 2) {
	return size_t(dst.v()) * width + size_t(dst.u());
}

template <typename T>
void vec_to_local(const onb<T>& onb, vec<T,3>& dst) noexcept {
	T a[3]{};
	for (size_t i = 0; i < 3; i++)
		a[i] = onb.axis[i] * dst;
	dst.set(a[0], a[1], a[2]);
}

template <typename T>
void vec_from_local(const onb<T>& onb, vec<T,3>& dst) noexcept {
	vec<T,3> v[3]{};
	for (size_t i = 0; i < 3; i++)
		v[i].set(onb.axis[i]) *= dst[i];
	dst  = v[0];
	dst += v[1];
	dst += v[2];
}

template <typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const vec<T,N>& v) {
	os << "(";
	for (size_t i = 0; i < N; i++) {
		if (i > 0) os << ", ";
		os << math::safe_format(v[i]);
	}
	os << ")";
	return os;
}

bool remove_separator_if(std::istringstream& is, char sep) {
	auto pos = is.tellg();
	char trash;
	is >> trash;
	if (trash == sep)
		return true;
	else
		is.seekg(pos);
	return false;
}

template <typename T, size_t N>
std::istringstream& operator>>(std::istringstream& is, vec<T,N>& v) {
	remove_separator_if(is, '(');
	for (size_t i = 0; i < N; i++) {
		is >> v[i];
		if (i + 1 != N && !remove_separator_if(is, ','))
			break;
	}
	remove_separator_if(is, ')');
	return is;
}

#endif // VEC_UTILS_HPP
