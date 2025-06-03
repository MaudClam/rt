#ifndef VEC_HPP
#define VEC_HPP

#include <cstddef>           // size_t
#include <cmath>             // std::sqrt
#include <ostream>           // for operator<<
#include <sstream>           // for operator>>
#include <cassert>           // assert
#include "math_utils.hpp"
#include "vec_utils.hpp"
#include "onb.hpp"


template <typename T, size_t N> struct vec;
template <typename T> struct onb;

using vec2d = vec<double,2>;
using vec2f = vec<float,2>;
using vec2i = vec<int,2>;
using vec3d = vec<double,3>;
using vec3f = vec<float,3>;
using vec3i = vec<int,3>;
using vec4d = vec<double,4>;
using vec4f = vec<float,4>;
using vec4i = vec<int,4>;


/// @brief Fixed-size mathematical vector with N components of type T.
///
/// Provides arithmetic operations, geometric transformations, and coordinate
/// conversions (e.g., Cartesian ↔ spherical) for vectors of arbitrary dimension N.
///
/// @tparam T Value type (e.g., float, double, int)
/// @tparam N Dimension of the vector (e.g., 2, 3, 4)
///
/// Features:
/// - Arithmetic: addition, subtraction, scaling, dot/cross product
/// - Geometric: normalization, reflection, refraction
/// - Coordinate transformations: Cartesian ↔ spherical (for N == 3)
/// - Scanline ↔ 2D screen mapping (for N == 2)
/// - Built on std::array<T, N> for STL-style access
///
/// @note For N == 3, additional spherical geometry utilities are available.
///
/// @warning Some methods (e.g., to_spherical) require T to be a floating-point type.
/// Using such methods with integral types (e.g., int) results in undefined or meaningless behavior.
///
/// @see vec_utils.hpp for related free functions and utilities.
template <typename T, size_t N>
struct vec {
	static_assert(std::is_arithmetic_v<T>, "vec<T, N>: T must be an arithmetic type");
	static_assert(N > 0, "vec<T,N>: N must be greater than 0");

	T e[N]{};

	constexpr vec() = default;
	template <typename... Args> requires (sizeof...(Args) == N)
	constexpr vec(Args... args) noexcept : e{ static_cast<T>(args)... } {}
	constexpr vec(const vec&) noexcept = default;
	~vec() = default;

	// ——— Access ———
	constexpr T& operator[](size_t i) noexcept {
	#ifndef NDEBUG
		assert(i < N);
	#endif
		return e[i];
	}
	constexpr T* begin() noexcept { return e; }
	constexpr T* end() noexcept { return e + N; }
	constexpr T& x() noexcept requires (N > 0) { return e[0]; }
	constexpr T& y() noexcept requires (N > 1) { return e[1]; }
	constexpr T& z() noexcept requires (N > 2) { return e[2]; }
	constexpr T& phi() noexcept requires (N > 0) { return e[0]; }
	constexpr T& theta() noexcept requires (N > 1) { return e[1]; }
	constexpr T& r() noexcept requires (N > 2) { return e[2]; }
	constexpr T& u() noexcept requires (N > 0) { return e[0]; }
	constexpr T& v() noexcept requires (N > 1) { return e[1]; }
	constexpr const T& operator[](size_t i) const noexcept {
	#ifndef NDEBUG
		assert(i < N);
	#endif
		return e[i];
	}
	constexpr const T* begin() const noexcept { return e; }
	constexpr const T* end() const noexcept { return e + N; }
	constexpr const T& x() const noexcept requires (N > 0) { return e[0]; }
	constexpr const T& y() const noexcept requires (N > 1) { return e[1]; }
	constexpr const T& z() const noexcept requires (N > 2) { return e[2]; }
	constexpr const T& phi() const noexcept requires (N > 0) { return e[0]; }
	constexpr const T& theta() const noexcept requires (N > 1) { return e[1]; }
	constexpr const T& r() const noexcept requires (N > 2) { return e[2]; }
	constexpr const T& u() const noexcept requires (N > 0) { return e[0]; }
	constexpr const T& v() const noexcept requires (N > 1) { return e[1]; }

	// ——— map ——— apply a function to all elements of a vector
	template <typename F>
	constexpr vec&	map(F&& f) noexcept(noexcept(f(e[0]))) {
		for (T& e_ : *this)
			e_ = f(e_);
		return *this;
	}

	// ——— zip_with ——— apply a binary operation to pairs of elements from two vectors
	template <typename F>
	constexpr vec&	zip_with(const vec& v, F&& f) noexcept(noexcept(f(e[0], v[0]))) {
		for (size_t i = 0; i < N; ++i)
			e[i] = f(e[i], v[i]);
		return *this;
	}

	// ——— lerp ——— makes the linearly interpolated point between a and b for a given t [0, 1]
	constexpr vec& lerp(const vec& a, const vec& b, T t) {
		for (size_t i = 0; i < N; ++i)
			(*this)[i] = (T(1) - t) * a[i] + t * b[i];
		return *this;
	}

	constexpr vec&	set(const vec& v) noexcept {
		if (this != &v)
			for (size_t i = 0; i < N; i++)
				this->e[i] = v.e[i];
		return *this; }
	constexpr vec&	set(T e0, T e1) noexcept requires(N == 2) { e[0] = e0; e[1] = e1; return *this; }
	constexpr vec&	set(T e0, T e1, T e2) noexcept requires(N == 3) { e[0] = e0; e[1] = e1; e[2] = e2; return *this; }
	constexpr vec&	set(T e0, T e1, T e2, T e3) noexcept requires(N == 4) { e[0] = e0; e[1] = e1; e[2] = e2; e[3] = e3; return *this; }
	constexpr vec&	fill(T f) noexcept { return map([f](T) { return f; }); }
	constexpr vec&	de_nan() noexcept { return map([](T x) { return math::de_nan<T>(x); }); }
	constexpr vec&	clamp(T lo, T hi) noexcept {
		for (T& e_ : *this)
			e_ = math::clamp<T>(e_, lo, hi);
		return *this;
	}
	constexpr vec&	operator=(const vec& o) noexcept { return this->set(o); }
	constexpr vec&	operator+=(const vec& o) noexcept { return zip_with(o, [](T x, T y) { return x + y; }); }
	constexpr vec&	operator-=(const vec& o) noexcept { return zip_with(o, [](T x, T y) { return x - y; }); }
	constexpr vec&	operator*=(T s) noexcept { return map([s](T x) { return x * s; }); }
	constexpr vec&	operator/=(T s) { return map([s](T x) { return math::safe_division<T>(x, s); }); }
	constexpr vec&	operator^=(const vec& o) noexcept requires(N == 3) { return set(vec().cross(*this,o)); }
	constexpr vec&	cross(const vec& a, const vec& b) noexcept requires (N == 3) {
		set(a[1]*b[2] - a[2]*b[1],
			a[2]*b[0] - a[0]*b[2],
			a[0]*b[1] - a[1]*b[0]);
		return *this;
	}
	constexpr vec&	normalize(T l = T(1)) noexcept {
		return	*this *= math::safe_division(l, length());
	}
	[[nodiscard]]
	constexpr T		length_squared() const noexcept {
		T sum = T(0);
		for (const T& e_ : *this)
			sum += e_ * e_;
		return sum;
	}
	[[nodiscard]]
	constexpr T		length() const noexcept { return std::sqrt(length_squared()); }
	[[nodiscard]]
	constexpr T		min() const noexcept {
		T m = e[0];
		for (size_t i = 1; i < N; ++i)
			if (e[i] < m)
				m = e[i];
		return m;
	}
	[[nodiscard]]
	constexpr T		max() const noexcept {
		T m = e[0];
		for (size_t i = 1; i < N; ++i)
			if (e[i] > m)
				m = e[i];
		return m;
	}
	[[nodiscard]]
	constexpr bool	is_null() const noexcept {
		for (const T& e : *this)
			if (e != T(0)) return false;
		return true;
	}

	// ——— Helpers ———
	constexpr vec& to_local(const onb<T>& onb) noexcept requires (N == 3) {
		vec_to_local<T>(onb, *this);
		return *this;
	}
	constexpr vec& from_local(const onb<T>& onb) noexcept requires (N == 3) {
		vec_from_local<T>(onb, *this);
		return *this;
	}
	constexpr vec& transform_between(const onb<T>& from, const onb<T>& to) noexcept requires (N == 3) {
		transform_between<T>(from, to, *this);
		return *this;
	}
	[[nodiscard]]
	constexpr vec<T,2> make_spherical_from_unit() const noexcept requires (N == 3) {
		return {
			math::spherical_phi<T>(y(), x()),
			math::spherical_theta<T>(z())
		};
	}
	constexpr vec& to_spherical() noexcept requires (N == 3) {
		to_spherical<T>(*this, *this);
		return *this;
	}
	constexpr vec& to_spherical(const vec& cartsn) noexcept requires (N == 3) {
		to_spherical<T>(cartsn, *this);
		return *this;
	}
	constexpr vec& from_spherical() noexcept requires (N == 3) {
		from_spherical<T>(*this, *this);
		return *this;
	}
	constexpr vec& from_spherical(const vec& sphercl) noexcept requires (N == 3) {
		from_spherical<T>(sphercl, *this);
		return *this;
	}
	constexpr vec& random_direction() noexcept requires (N == 3) {
		random_direction<T>(*this);
		return *this;
	}
	constexpr vec& random_direction_in_hemisphere(const vec& normal) noexcept requires (N == 3) {
		random_direction_<T>() += normal;
		return this->normalize();
	}
	constexpr vec& random_cosine_direction(void) noexcept requires (N == 3) {
		random_cosine_direction<T>(*this);
		return *this;
	}
	constexpr vec& random_cosine_direction(const onb<T>& onb) noexcept requires (N == 3) {
		random_cosine_direction<T>(*this);
		return to_local(onb);
	}
	constexpr vec& random_cosine_direction(const vec& normal) noexcept requires (N == 3) {
		return random_cosine_direction(onb<T>().build(normal));
	}
	constexpr vec& rotate_xy(T angle) noexcept requires (N >= 2) { return rotate<T>(x(), y(), angle); }
	constexpr vec& rotate_yz(T angle) noexcept requires (N >= 3) { return rotate<T>(y(), z(), angle); }
	constexpr vec& rotate_zx(T angle) noexcept requires (N >= 3) { return rotate<T>(z(), x(), angle); }
	constexpr vec& rotate_around_axis(const vec& axis, T angle) noexcept requires (N >= 3) {
		rotate_around_axis<T>(axis, angle, *this);
		return *this;
	}
	constexpr vec& reflect_direction(const vec& normal) noexcept requires (N == 3) {
		reflect_direction<T>(normal, *this);
		return *this;
	}
	[[nodiscard]]
	constexpr T refract_schlick_direction(const vec& normal, T eta) noexcept requires (N == 3) {
		return refract_schlick_direction<T>(normal, eta, *this);
	}
	constexpr vec& to_screen(T width, T height) noexcept requires (N >= 2) {
		to_screen<T>(width, height, *this);
		return *this;
	}
	constexpr vec& from_screen(T width, T height) noexcept requires (N >= 2) {
		from_screen<T>(width, height, *this);
		return *this;
	}
	constexpr vec& line_to_screen(size_t width, size_t index) noexcept requires (N >= 2) {
		line_to_screen<T>(width, index, *this);
		return *this;
	}
	[[nodiscard]]
	constexpr size_t screen_to_line(size_t width) const noexcept requires (N == 2) {
		return screen_to_line<T>(width, *this);
	}
	[[nodiscard]]
	std::string output() {
		std::ostringstream os;
		os << *this;
		return os.str();
	}
	template <typename> friend constexpr std::istringstream& operator>>(std::istringstream& is, vec& v);
	template <typename> friend std::ostream& operator<<(std::ostream& os, const vec& v);
};


#endif
