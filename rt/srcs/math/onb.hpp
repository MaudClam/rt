#ifndef ONB_HPP
#define ONB_HPP

#include <cassert>
#include "math_utils.hpp"
#include "vec.hpp"
#include "vec_utils.hpp"

template <typename T>
struct onb {
	using vec = vec<T,3>;
	union {
		vec axis[3];
		struct { vec t, b, n; };
	};

	constexpr onb() = default;
	constexpr onb(const onb&) noexcept = default;
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
	constexpr vec from_local(const onb& onb) noexcept {
		if (this != &onb)
			for (vec a : *this)
				vec_to_local(onb, a);
		return *this;
	}

	/// @brief Validates orthonormality of the basis (for debug).
	[[nodiscard]]
	bool is_orthonormal(T eps = math::constants<T>::eps) const noexcept {
		return std::abs(dot(t, b)) < eps &&
			   std::abs(dot(t, n)) < eps &&
			   std::abs(dot(b, n)) < eps &&
			   std::abs(t.length_squared() - T(1)) < eps &&
			   std::abs(b.length_squared() - T(1)) < eps &&
			   std::abs(n.length_squared() - T(1)) < eps;
	}

	/// @brief Rotate t and b around n.
	constexpr onb& yaw(T angle) noexcept {
		rotate(t, b, math::wrap_angle(angle), n);
		return *this;
	}
	
	/// @brief Rotate n and t around b.
	constexpr onb& pitch(T angle) noexcept {
		rotate(n, t, math::wrap_angle(angle), b);
		return *this;
	}
	
	/// @brief Rotate n and b around t.
	constexpr onb& roll(T angle) noexcept {
		rotate(n, b, math::wrap_angle(angle), t);
		return *this;
	}

private:
	constexpr void rotate(vec& a, vec& b, T angle, const vec& axis) noexcept {
		const T sin = std::sin(angle), cos = std::cos(angle);
		rotate_around_axis(axis, sin, cos, a);
		rotate_around_axis(axis, sin, cos, b);
	}
};

#endif // ONB_HPP

