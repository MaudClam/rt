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

# define PRECISION		1e-9
# define EPSILON 		1e-3

enum  Hit { FRONT, BACK, OUTLINE, FRONT_SHADOW };
bool  almostEqual(float a, float b, int precision = PRECISION);
bool  almostEqual(double a, double b, int precision = PRECISION);
float radian(float degree);
float degree(float radian);

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
	Vec2<t>& toMonitor(int width, int height) { x += width / 2; y = height / 2 - y - 1; return *this; }
	Vec2<t>& toRt(int width, int height) { x -= width / 2; y = height / 2 - y - 1; return *this; }
	inline bool isNull(void) { return x == 0 && y == 0;  }
	template <class > friend std::ostream& operator<<(std::ostream& o, Vec2<t>& v);
	template <class > friend std::istringstream& operator>>(std::istringstream& is, Vec2<t>& v);
};

template <class t> std::ostream& operator<<(std::ostream& o, Vec2<t>& v) {
	std::ostringstream os;
	os << v.x << ",";
	o << std::setw(6) << std::right << std::setfill(' ') << os.str()
	  << std::setw(5) << std::left << std::setfill(' ') << v.y;
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
	Vec3<t>& product(const Vec3<t>& v1, const Vec3<t>& v2) {
		t _x = v1.y * v2.z - v1.z * v2.y;
		t _y = v1.z * v2.x - v1.x * v2.z;
		t _z = v1.x * v2.y - v1.y * v2.x;
		x = _x; y =_y; z = _z;
		return *this;
	}
	inline Vec3<t>	operator+(const Vec3<t>& v) const {
		return Vec3<t>(x + v.x, y + v.y, z + v.z);
	}
	Vec3<t>& addition(const Vec3<t>& v1, const Vec3<t>& v2) {
		x = v1.x + v2.x;
		y = v1.y + v2.y;
		z = v1.z + v2.z;
		return *this;
	}
	inline Vec3<t>	operator-(const Vec3<t>& v) const {
		return Vec3<t>(x - v.x, y - v.y, z - v.z);
	}
	Vec3<t>& substract(const Vec3<t>& v1, const Vec3<t>& v2) {
		x = v1.x - v2.x;
		y = v1.y - v2.y;
		z = v1.z - v2.z;
		return *this;
	}
	inline Vec3<t>	operator*(float f) const { return Vec3<t>(x * f, y * f, z * f); }
	Vec3<t>& product(float f)  { x *= f; y *= f; z *= f; return *this; }
	inline t operator*(const Vec3<t>& v) const { return x * v.x + y * v.y + z * v.z;}
	inline t product(const Vec3<t>& v) const { return x * v.x + y * v.y + z * v.z;}
	inline float norm () const {
		return std::sqrt(x * x + y * y + z * z);
	}
	Vec3<t>& normalize(t l=1) {
		t _norm = norm();
		if (_norm != 0) {
			product(l / _norm);
		}
		return *this;
	}
	Vec3<t>& reflect(const Vec3<t>& norm) {
		product(-1);
		substract(norm * (*this * norm * 2), *this).normalize();
		return *this;
	}
	Vec3<t>& lookatDir(const LookatAuxiliary<t>& aux) {
		t _x = *this * aux.right, _y = *this * aux.up, _z = *this * aux.dir;
		x = _x; y = _y; z = _z;
		this->normalize();
		return *this;
	}
	Vec3<t>& lookatPt(const Vec3<t>& eyePt, const LookatAuxiliary<t>& aux) {
		substract(*this, eyePt);
		t _x = *this * aux.right, _y = *this * aux.up, _z = *this * aux.dir;
		x = _x; y = _y; z = _z;
		return *this;
	}
	Vec3<t>& turnAroundX(float angle) {
		if ( !(angle == 0 || (y == 0 && z == 0)) ) {
			float sin = std::sin(angle), cos = std::cos(angle);
			float _z = z * cos - y * sin;
			float _y = z * sin + y * cos;
			y = _y; z = _z;
		}
		return *this;
	}
	inline Vec3<t> turnAround_X(float angle) {
		return Vec3<t>(*this).turnAroundX(angle);
	}
	Vec3<t>& turnAroundY(float angle) {
		if ( !(angle == 0 || (x == 0 && z == 0)) ) {
			float sin = std::sin(angle), cos = std::cos(angle);
			float _z = z * cos - x * sin;
			float _x = z * sin + x * cos;
			x = _x; z = _z;
		}
		return *this;
	}
	inline Vec3<t> turnAround_Y(float angle) {
		return Vec3<t>(*this).turnAroundY(angle);
	}
	Vec3<t>& turnAroundZ(float angle) {
		if ( !(angle == 0 || (x == 0 && y == 0)) ) {
			float sin = std::sin(angle), cos = std::cos(angle);
			float _x = x * cos - y * sin;
			float _y = x * sin + y * cos;
			x = _x; y = _y;
		}
		return *this;
	}
	inline Vec3<t> turnAround_Z(float angle) {
		return Vec3<t>(*this).turnAroundZ(angle);
	}
	inline bool isNull(void) const { return x == 0 && y == 0 && z == 0;  }
	inline void toNull(void) { x = 0; y = 0; z = 0;  }
	Vec3<t>& toRt(int width, int height) { x -= width / 2; y = height / 2 - y - 1; return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
	template <class > friend std::istringstream& operator>>(std::istringstream& is, Vec3<t>& v);
};

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	std::ostringstream os, os1;
	os  << v.x << ",";
	os1 << v.y << "," << v.z;
	s << std::setw(5) << std::right << std::setfill(' ') << os.str();
	s << std::setw(8) << std::left << std::setfill(' ') << os1.str();
	return s;
}

template <class t> std::istringstream& operator>>(std::istringstream& is, Vec3<t>& v) {
	char trash;
	v.x = 0; v.y = 0; v.z = 0;
	is >> v.x >> trash >> v.y >> trash >> v.z;
	return is;
}

template <class t> bool operator==(const Vec3<t>& lhs, const Vec3<t>& rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

template <class t> bool operator!=(const Vec3<t>& lhs, const Vec3<t>& rhs) {
	return !(lhs == rhs);
}

template <class t> struct LookatAuxiliary {
	Vec3<t> dir;
	Vec3<t> up;
	Vec3<t> right;
	LookatAuxiliary(const Vec3<t>& eyeDir) : dir(eyeDir), up(), right(0,-1,0)
	{
		if ( dir.x == 0 && (dir.y == -1 || dir.y == 1) && dir.z == 0) {
			right.y = 0; right.z = 1;
		}
		right.product(dir,right).normalize();
		up.product(dir,right).normalize();
	}
	~LookatAuxiliary(void) {}
};

typedef Vec2<float>				Vec2f;
typedef Vec2<int>				Vec2i;
typedef Vec3<float>				Vec3f;
typedef Vec3<int>				Vec3i;
typedef LookatAuxiliary<float>	LookatAux;

struct Position {
	Vec3f	p;
	Vec3f	n;
	Position(void);
	~Position(void);
	Position(const Vec3f& point, const Vec3f& norm);
	Position(const Position& other);
	Position& operator=(const Position& other);
	Position& lookat(const Position& eye);
	Position& lookat(const Position& eye, const LookatAux& aux);
};


// Intersections, normals, rays

bool raySphereIntersection(const Vec3f& rayDir,
						   const Vec3f& rayPov,
						   const Vec3f& center,
						   float sqrRadius,
						   float& distance,
						   Hit rayHit = FRONT);

bool raySphereIntersection(const Vec3f& rayDir,
							const Vec3f& k,
							float c,
							float& distance,
							Hit rayHit = FRONT);

void normalToRaySphereIntersect(const Vec3f& intersectPt,
								const Vec3f& center,
								Vec3f& normal);

bool rayPlaneIntersection(const Vec3f& pov,
						  const Vec3f& dir,
						  const Vec3f& point,
						  const Vec3f& norm,
						  float& distance);

#endif
