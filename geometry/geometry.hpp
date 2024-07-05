//
//  geometry.hpp
//  geometry
//
//  Created by uru on 02/07/2024.
//

#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

# include <iostream>
# include <sstream>
# include <cmath>

template <class t> struct Vec2 {
	union {
		struct {t u, v;};
		struct {t x, y;};
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u),v(_v) {}
	inline Vec2<t> operator+(const Vec2<t>& V) const { return Vec2<t>(u + V.u, v + V.v); }
	inline Vec2<t> operator-(const Vec2<t>& V) const { return Vec2<t>(u - V.u, v - V.v); }
	inline Vec2<t> operator*(float f)          const { return Vec2<t>(u * f, v * f); }
	template <class > friend std::ostream& operator<<(std::ostream& o, Vec2<t>& v);
	template <class > friend std::istringstream& operator>>(std::istringstream& is, Vec2<t>& v);
};

template <class t> std::ostream& operator<<(std::ostream& o, Vec2<t>& v) {
	o << v.x << "," << v.y;
	return o;
}

template <class t> std::istringstream& operator>>(std::istringstream& is, Vec2<t>& v) {
	char trash;
	v.x = 0; v.y = 0;
	is >> v.x >> trash >> v.y;
	return is;
}

template <class t> struct Vec3 {
	union {
		struct {t x, y, z;};
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
	inline Vec3<t> operator^(const Vec3<t>& v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	inline Vec3<t> operator+(const Vec3<t>& v) const { return Vec3<t>(x + v.x, y + v.y, z + v.z); }
	inline Vec3<t> operator-(const Vec3<t>& v) const { return Vec3<t>(x - v.x, y - v.y, z - v.z); }
	inline Vec3<t> operator*(float f)          const { return Vec3<t>(x * f, y * f, z * f); }
	inline t       operator*(const Vec3<t>& v) const { return x * v.x + y * v.y + z * v.z; }
	float norm () const { return std::sqrt(x * x + y * y + z * z); }
	Vec3<t>& normalize(t l=1) { *this = (*this) * (l / norm()); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
	template <class > friend std::istringstream& operator>>(std::istringstream& is, Vec3<t>& v);
};

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << v.x << "," << v.y << "," << v.z;
	return s;
}

template <class t> std::istringstream& operator>>(std::istringstream& is, Vec3<t>& v) {
	char trash;
	v.x = 0; v.y = 0; s.z = 0;
	is >> v.x >> trash >> v.y >> trash >> v.z;
	return is;
}

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

#endif
