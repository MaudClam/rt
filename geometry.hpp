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
# include <iomanip>
# include <cmath>
# include <numbers>

# define BASE_PT	Vec3f(0,0,0)
# define BASE_DIR	Vec3f(0,0,1)
# define BASE		BASE_PT,BASE_DIR
# define PRECISION	1e-9
# define EPSILON 	1e-5


bool almostEqual(float a, float b, int precision = PRECISION);
bool almostEqual(double a, double b, int precision = PRECISION);

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
	inline Vec2<t>& toMonitor(int width, int height) { x += width / 2; y = height / 2 - y - 1; return *this; }
	inline Vec2<t>& toRt(int width, int height) { x -= width / 2; y = height / 2 - y - 1; return *this; }
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

template <class t> struct LookatAuxiliary;

template <class t> struct Vec3 {
	union {
		struct {t x, y, z;};
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
	inline Vec3<t>	operator^(const Vec3<t>& v) const {
		return Vec3<t>(y * v.z - z * v.y,
					   z * v.x - x * v.z,
					   x * v.y - y * v.x);
	}
	inline Vec3<t>&	product(const Vec3<t>& v1, const Vec3<t>& v2) {
		t _x = v1.y * v2.z - v1.z * v2.y;
		t _y = v1.z * v2.x - v1.x * v2.z;
		t _z = v1.x * v2.y - v1.y * v2.x;
		x = _x; y =_y; z = _z;
		return *this;
	}
	inline Vec3<t>	operator+(const Vec3<t>& v) const {
		return Vec3<t>(x + v.x, y + v.y, z + v.z);
	}
	inline Vec3<t>	operator-(const Vec3<t>& v) const {
		return Vec3<t>(x - v.x, y - v.y, z - v.z);
	}
	inline Vec3<t>&	substract(const Vec3<t>& v1, const Vec3<t>& v2) {
		x = v1.x - v2.x;
		y = v1.y - v2.y;
		z = v1.z - v2.z;
		return *this;
	}
	inline Vec3<t>	operator*(float f) const { return Vec3<t>(x * f, y * f, z * f); }
	inline t		operator*(const Vec3<t>& v) const { return x * v.x + y * v.y + z * v.z;}
	inline float 	norm () const {
		return std::sqrt(x * x + y * y + z * z);
	}
	inline Vec3<t>&	normalize(t l=1) {
		t _norm = norm();
		if (_norm != 0) {
			*this = (*this) * (l / _norm);
		}
		return *this;
	}
	inline Vec3<t>& lookatDir(const LookatAuxiliary<t>& aux) {
		t _x = *this * aux.right, _y = *this * aux.up, _z = *this * aux.dir;
		x = _x; y = _y; z = _z;
		this->normalize();
		return *this;
	}
	inline Vec3<t>& lookatPt(const Vec3<t>& eyePt, const LookatAuxiliary<t>& aux) {
		substract(*this, eyePt);
		t _x = *this * aux.right, _y = *this * aux.up, _z = *this * aux.dir;
		x = _x; y = _y; z = _z;
		return *this;
	}
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
	template <class > friend std::istringstream& operator>>(std::istringstream& is, Vec3<t>& v);
};

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << v.x << "," << v.y << "," << v.z;
	return s;
}

template <class t> std::istringstream& operator>>(std::istringstream& is, Vec3<t>& v) {
	char trash;
	v.x = 0; v.y = 0; v.z = 0;
	is >> v.x >> trash >> v.y >> trash >> v.z;
	return is;
}

template <class t> bool operator==(const Vec3<t>& lhs, const Vec3<t>& rhs) {
	return almostEqual(lhs.x, rhs.x) && almostEqual(lhs.y, rhs.y) && almostEqual(lhs.z, rhs.z);
}

template <class t> bool operator!=(const Vec3<t>& lhs, const Vec3<t>& rhs) {
	return !(lhs == rhs);
}

template <class t> struct LookatAuxiliary {
	Vec3<t> dir;
	Vec3<t> up;
	Vec3<t> right;
	LookatAuxiliary(const Vec3<t>& eyeDir) : dir(eyeDir), up(), right(0,-1,0) {
		if (
			almostEqual(dir.x, 0) &&
			( almostEqual(dir.y, -1) || almostEqual(dir.y, 1) ) &&
			almostEqual(dir.z, 0)
			)
		{
			right.y = 0; right.z = -1;
		}
		right.product(dir, right); right.normalize();
		up.product(dir, right); up.normalize();
	}
	~LookatAuxiliary(void) {}
};

typedef Vec2<float> 		Vec2f;
typedef Vec2<int>   		Vec2i;
typedef Vec3<float> 		Vec3f;
typedef Vec3<int>   		Vec3i;
typedef LookatAuxiliary<float>	LookatAux;

struct Position {
	Vec3f	p;
	Vec3f	n;
	Position(void);
	~Position(void);
	Position(const Vec3f& point, const Vec3f& nnorm);
	Position(const Position& other);
	Position& operator=(const Position& other);
	Position& lookat(const Position& eye);
	Position& lookat(const Position& eye, const LookatAux& aux);
	Position& rollingDegree(float roll);
};


// Non member functions

float radian(float degree);
float degree(float radian);

#endif
