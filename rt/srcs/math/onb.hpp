#ifndef ONB_HPP
#define ONB_HPP

#include "math_constants.hpp"
#include "math_utils.hpp"
#include "vec.hpp"
#include "angle.hpp"
#include "vec_utils.hpp"

template <typename T, size_t N> struct vec;

using namespace math;

template <typename T>
struct onb {
	using vec = vec<T,3>;
	using angle = angle<T>;
	union {
		vec axis[3];
		struct { vec t, b, n; };
	};

	constexpr onb() noexcept :
	axis{
		{T(1), T(0), T(0)},
		{T(0), T(1), T(0)},
		{T(0), T(0), T(1)}
	} {}
	
	constexpr onb(const onb& o) noexcept : axis{} { *this = o; }

	constexpr onb& operator=(const onb& o) noexcept {
		if (this != &o)
			for (size_t i = 0; i < 3; i++)
				axis[i] = o.axis[i];
		return *this;
	}
	
	onb(onb&&) = delete;
	onb& operator=(onb&&) = delete;
	~onb() = default;
	
	// ——— Access ———
	constexpr vec* begin() noexcept { return axis; }
	constexpr vec* end() noexcept { return axis + 3; }
	constexpr const vec* begin() const noexcept { return axis; }
	constexpr const vec* end() const noexcept { return axis + 3; }


	/// @brief Builds an ONB from a normal (builds t and b accordingly).
	constexpr onb& build(const vec& normal) {
		n = normal;
		if (std::abs(n.z()) < T(0.999))
			t.cross(n, {0,0,1}).normalize();
		else
			t.cross(n, {0,1,0}).normalize();
		b.cross(n, t);
		return *this;
	}

	/// @brief Transforms *this from world space to local ONB space.
	constexpr onb& to_local(const onb& onb) noexcept {
		if (this != &onb)
			for (vec a : *this)
				vec_from_local(onb, a);
		return *this;
	}
	
	/// @brief Transforms *this from local ONB space to world space.
	constexpr onb& from_local(const onb& onb) noexcept {
		if (this != &onb)
			for (vec a : *this)
				vec_to_local(onb, a);
		return *this;
	}

	/// @brief Validates orthonormality of the basis (for debug).
	[[nodiscard]]
	bool is_orthonormal(T eps = constants<T>::eps) const noexcept {
		return std::abs(dot(t, b)) < eps &&
			   std::abs(dot(t, n)) < eps &&
			   std::abs(dot(b, n)) < eps &&
			   std::abs(t.length_squared() - T(1)) < eps &&
			   std::abs(b.length_squared() - T(1)) < eps &&
			   std::abs(n.length_squared() - T(1)) < eps;
	}

	/// @brief Rotate t and b around n.
	constexpr onb& yaw(T tilt) noexcept {
		rotate(t, b, wrap_angle<T>(tilt), n);
		return *this;
	}
	constexpr onb& yaw(const angle& tilt) noexcept {
		rotate(t, b, tilt, n);
		return *this;
	}

	/// @brief Rotate n and t around b.
	constexpr onb& pitch(T tilt) noexcept {
		rotate(n, t, wrap_angle<T>(tilt), b);
		return *this;
	}
	constexpr onb& pitch(const angle& tilt) noexcept {
		rotate(n, t, tilt, b);
		return *this;
	}

	/// @brief Rotate n and b around t.
	constexpr onb& roll(T tilt) noexcept {
		rotate(n, b, wrap_angle<T>(tilt), t);
		return *this;
	}
	constexpr onb& roll(const angle& tilt) noexcept {
		rotate(n, b, tilt, t);
		return *this;
	}

private:
	constexpr void rotate(vec& a, vec& b, T tilt, const vec& axis) noexcept {
		const T sin = std::sin(tilt), cos = std::cos(tilt);
		rotate_around_axis<T>(axis, sin, cos, a);
		rotate_around_axis<T>(axis, sin, cos, b);
	}
	constexpr void rotate(vec& a, vec& b, const angle& tilt, const vec& axis) noexcept {
		rotate_around_axis<T>(axis, tilt.sin, t.cos, a);
		rotate_around_axis<T>(axis, tilt.sin, t.cos, b);
	}
};

#endif // ONB_HPP

