#ifndef ANGLE_HPP
#define ANGLE_HPP

#include <cmath>
#include "math_utils.hpp"

namespace math {

template <typename T>
struct angle {
	union {
		struct { T cos, sin; };
		struct { T x, y; };
	};

	constexpr angle() noexcept : cos(T(1)), sin(T(0)) {}
	constexpr angle(const angle& o) noexcept : angle() { set(o); }
	constexpr angle(T r) noexcept : angle() { set(r); }

//	angle(angle&&) = delete;
//	angle& operator=(angle&&) = delete;
	~angle() = default;

	constexpr angle& set(T r) noexcept {
		r = wrap_angle<T>(r);
		cos = std::cos(r);
		sin = std::sin(r);
		return *this;
	}
	constexpr angle& set(const angle& o) noexcept {
		if (this != &o) {
			cos = o.cos;
			sin = o.sin;
		}
		return *this;
	}
	constexpr angle& set_degrees(T d) noexcept {
		return set(to_radians<T>(d));
	}
	[[nodiscard]]
	constexpr T radians() const noexcept {
		using C = constants<T>;
//		if (almost_equal(T(0)))
//			return T(0);
		if (almost_equal(C::pi))
			return -C::pi;
		return std::atan2(y, x);
//		return safe_format(std::atan2(y, x));
	}
	[[nodiscard]]
	constexpr T degrees() const noexcept {
		return to_degrees<T>(radians());
	}

	constexpr angle& operator=(const angle& o) noexcept { return set(o); }
	constexpr angle& operator=(T r) noexcept { return set(r); }
	constexpr angle& operator+=(const angle& o) noexcept {
		const T cos_ = cos * o.cos - sin * o.sin;
		const T sin_ = sin * o.cos + cos * o.sin;
		cos = cos_; sin = sin_;
		return *this;
	}
	constexpr angle& operator+=(T r) noexcept {
		return *this += angle(r);
	}
	constexpr angle& operator-=(const angle& o) noexcept {
		const T cos_ = cos * o.cos + sin * o.sin;
		const T sin_ = sin * o.cos - cos * o.sin;
		cos = cos_; sin = sin_;
		return *this;
	}
	constexpr angle& operator-=(T r) noexcept {
		return *this -= angle(r);
	}
	constexpr angle& operator*=(T s) noexcept {
		return set_radians(angle() * s);
	}
	constexpr angle& operator-() noexcept {
		y = -y;
		return *this;
	}

	[[nodiscard]]
	constexpr bool almost_equal(const angle& o, T eps = constants<T>::eps) const noexcept {
		return std::abs(cos * o.cos + sin * o.sin - T(1)) < eps;
	}
	[[nodiscard]]
	constexpr bool almost_equal(T r, T eps = constants<T>::eps) const noexcept {
		return almost_equal(angle().set(r), eps);
	}
	[[nodiscard]]
	constexpr bool compare(const angle& o) const noexcept {
		return cos * o.sin - sin * o.cos < T(0);
	}
	[[nodiscard]]
	constexpr bool compare(T r) const noexcept {
		return compare(angle(r));
	}
	[[nodiscard]]
	std::string output() const noexcept {
		std::ostringstream os;
		os << *this;
		return os.str();
	}
	[[nodiscard]]
	std::string output_degrees() const noexcept {
		std::ostringstream os;
		os << safe_format<T>( to_degrees<T>(radians()) ) << "º";
		return os.str();
	}
	template <typename> friend constexpr std::istringstream& operator>>(std::istringstream& is, angle& a);
	template <typename> friend std::ostream& operator<<(std::ostream& os, const angle& a);
};

template <typename T>
[[nodiscard]]
constexpr angle<T> operator+(const angle<T>& a, const angle<T>& b) noexcept {
return angle<T>().set(a) += b;
}
template <typename T>
[[nodiscard]]
constexpr angle<T> operator+(const angle<T>& t, T r) noexcept {
	return angle<T>().set(t) += r;
}
template <typename T>
[[nodiscard]]
constexpr angle<T> operator+(T r, const angle<T>& t) noexcept {
	return t + r;
}
template <typename T>
[[nodiscard]]
constexpr angle<T> operator-(const angle<T>& a, const angle<T>& b) noexcept {
	return angle<T>().set(a) -= b;
}
template <typename T>
[[nodiscard]]
constexpr angle<T> operator-(const angle<T>& a, T r) noexcept {
	return angle<T>().set(a) -= r;
}
template <typename T>
[[nodiscard]]
constexpr angle<T> operator-(T r, const angle<T>& a) noexcept {
	return a - r;
}
template <typename T>
[[nodiscard]]
constexpr angle<T> operator*(T r, const angle<T>& a) noexcept {
	return angle<T>().set(a) *= r;
}

template <typename T>
[[nodiscard]]
constexpr bool  operator==(const angle<T>& a, T r) noexcept {
	return a.almost_equal(r);
}
template <typename T>
[[nodiscard]]
constexpr bool  operator==(T r, const angle<T>& a) noexcept {
	return a == r;
}
template <typename T>
[[nodiscard]]
constexpr bool  operator==(const angle<T>& a, const angle<T>& b) noexcept {
	return a.almost_equal(b);
}
template <typename T>
[[nodiscard]]
constexpr bool  operator!=(const angle<T>& a, T r) noexcept {
	return !(a == r);
}
template <typename T>
[[nodiscard]]
constexpr bool  operator!=(T r, const angle<T>& a) noexcept {
	return !(a == r);
}
template <typename T>
[[nodiscard]]
constexpr bool  operator!=(const angle<T>& a, const angle<T>& b) noexcept {
	return !(a == b);
}

template <typename T>
[[nodiscard]]
constexpr bool  operator<(const angle<T>& a, T r) noexcept {
	return a.compare(r);
}
template <typename T>
[[nodiscard]]
constexpr bool  operator>(const angle<T>& a, T r) noexcept {
	return angle<T>().set(r).compare(a);
}
template <typename T>
[[nodiscard]]
constexpr bool  operator<=(const angle<T>& a, T r) noexcept {
	return !(a > r);
}
template <typename T>
[[nodiscard]]
constexpr bool  operator>=(const angle<T>& a, T r) noexcept {
	return !(a < r);
}

template <typename T>
[[nodiscard]]
constexpr bool  operator<(T r, const angle<T>& a) noexcept {
	return a > r;
}
template <typename T>
[[nodiscard]]
constexpr bool  operator>(T r, const angle<T>& a) noexcept {
	return a < r;
}
template <typename T>
[[nodiscard]]
constexpr bool  operator<=(T r, const angle<T>& a) noexcept {
	return !(r > a);
}
template <typename T>
[[nodiscard]]
constexpr bool  operator>=(T r, const angle<T>& a) noexcept {
	return !(r < a);
}

template <typename T>
[[nodiscard]]
constexpr bool  operator<(const angle<T>& a, const angle<T>& b) noexcept {
	return a.compare(b);
}
template <typename T>
[[nodiscard]]
constexpr bool  operator>(const angle<T>& a, const angle<T>& b) noexcept {
	return b < a;
}
template <typename T>
[[nodiscard]]
constexpr bool  operator<=(const angle<T>& a, const angle<T>& b) noexcept {
	return !(a > b);
}
template <typename T>
[[nodiscard]]
constexpr bool  operator>=(const angle<T>& a, const angle<T>& b) noexcept {
	return !(a < b);
}

template <typename T>
constexpr std::istringstream& operator>>(std::istringstream& is, angle<T>& a) noexcept {
	T x = 0;
	is >> x;
	if (remove_separator_if(is, "º"))
		a.set_degree(x);
	else
		a.set(x);
	return is;
}
template <typename T>
std::ostream& operator<<(std::ostream& os, const angle<T>& a) noexcept {
	os << safe_format<T>(a.radians());
	return os;
}


} // namespace math

#endif /* ANGLE_HPP */
