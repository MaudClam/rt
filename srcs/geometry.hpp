#ifndef GEOMETRY_HPP
# define GEOMETRY_HPP

# include <iomanip>
# include <sstream>
# include <functional>
# include <random>
# include "Header.h"

template <class t> struct Vec2;
template <class t> struct Vec3;
template <class t> struct LookatAuxiliary;

typedef Vec2<float>				Vec2f;
typedef Vec2<int>				Vec2i;
typedef Vec3<double>			Vec3d;
typedef Vec3<float>				Vec3f;
typedef Vec3<int>				Vec3i;
typedef LookatAuxiliary<float>	LookatAux;

const float M_2PI(2. * M_PI);
const float M_PI_180(M_PI / 180.);
const float M_180_PI(180. / M_PI);

enum Hit { FRONT, BACK, OUTLINE, ANY_SHADOW, ALL_SHADOWS, INSIDE, OUTSIDE, IN_VOLUME };
enum CombineType { END=0, UNION, SUBTRACTION, INTERSECTION };
enum MapType {NO, CAUSTIC, GLOBAL, VOLUME, RESET};
enum TracingType { RAY, PATH };


// Non class functions
double	random_double(void);
bool	almostEqual(float a, float b, float precision = PRECISION);
float	degree2radian(float degree);
float	radian2degree(float radian);
float	getShining(const Vec3f& dirFromPov, const Vec3f& normal, const Vec3f& dirToLight, float glossy);
float	getShining_(const Vec3f& dirFromPov, const Vec3f& normal, Vec3f dirToLight, float glossy);
float	getSchlick(float cosine, float ref_idx);
std::string roundedString(float num, int factor = 2);
std::string combineType(CombineType type);
std::string mapType(MapType);
template <class t> t deNaN(t n) { return !(n == n) ? 0 : n; }


// struct Vec2
template <class t> struct Vec2 {
	union {
		struct { t u, v; };
		struct { t x, y; };
		t raw[2];
	};
	Vec2(void) : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u),v(_v) {}
	Vec2(const Vec2<t>& other) : x(other.x), y(other.y) {}
	~Vec2(void) {}
	Vec2<t>& operator=(const Vec2<t>& other) {
		if (this != &other) {
			x = other.x;
			y = other.y;
		}
		return *this;
	}
	Vec2<t>& set_xy(t _x, t _y) { x = _x; y = _y; return *this; }
	inline Vec2<t> operator+(const Vec2<t>& V) const { return Vec2<t>(u + V.u, v + V.v); }
	inline Vec2<t> operator-(const Vec2<t>& V) const { return Vec2<t>(u - V.u, v - V.v); }
	inline Vec2<t> operator*(float f)          const { return Vec2<t>(u * f, v * f); }
	Vec2<t>& toMonitor(int width, int height) { x += width / 2; y = height / 2 - y - 1; return *this; }
	Vec2<t>& toRt(int width, int height) { x -= width / 2; y = height / 2 - y - 1; return *this; }
	inline bool isNull(void) { return x == 0 && y == 0;  }
	template <class > friend std::ostream& operator<<(std::ostream& o, const Vec2<t>& v);
	template <class > friend std::istringstream& operator>>(std::istringstream& is, Vec2<t>& v);
	template <class > friend bool operator==(const Vec2<t>& left, const Vec2<t>& right);
	template <class > friend bool operator!=(const Vec2<t>& left, const Vec2<t>& right);
};
template <class t> std::ostream& operator<<(std::ostream& o, const Vec2<t>& v) {
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
template <class t> bool operator==(const Vec2<t>& left, const Vec2<t>& right) {
	return left.x == right.x && left.y == right.y;
}
template <class t> bool operator!=(const Vec2<t>& left, const Vec2<t>& right) {
	return !(left == right);
}
// struct Vec2 end


// struct Vec3
template <class t> struct Vec3 {
	union {
		struct { t x, y, z; };
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3(void) : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
	Vec3(const Vec3<t>& other) : x(other.x), y(other.y), z(other.z) {}
	~Vec3(void) {}
	Vec3<t>& operator=(const Vec3<t>& other) {
		if (this != &other) {
			for (int i = 0; i < 3; i++)
				raw[i] = other.raw[i];
		}
		return *this;
	}
	Vec3<t>& set_xyz(t _x, t _y, t _z) { x = _x; y = _y; z = _z; return *this; }
	Vec3<t>& product(const Vec3<t>& v1, const Vec3<t>& v2) {
		t _x = v1.y * v2.z - v1.z * v2.y;
		t _y = v1.z * v2.x - v1.x * v2.z;
		t _z = v1.x * v2.y - v1.y * v2.x;
		x = _x; y =_y; z = _z;
		return *this;
	}
	Vec3<t>& addition(const Vec3<t>& v1, const Vec3<t>& v2) {
		x = v1.x + v2.x;
		y = v1.y + v2.y;
		z = v1.z + v2.z;
		return *this;
	}
	Vec3<t>& substract(const Vec3<t>& v1, const Vec3<t>& v2) {
		x = v1.x - v2.x;
		y = v1.y - v2.y;
		z = v1.z - v2.z;
		return *this;
	}
	Vec3<t>& product(float f)  { x *= f; y *= f; z *= f; return *this; }
	Vec3<t>& normalize(t l = 1) {
		t _norm = norm();
		if (_norm != 0) {
			product(l / _norm);
		}
		return *this;
	}
	inline t product(const Vec3<t>& v) const { return x * v.x + y * v.y + z * v.z;}
	inline float sqnorm(void) const { return x * x + y * y + z * z; }
	inline float norm(void) const { return std::sqrt(sqnorm()); }
	inline bool  isNull(void) const { return x == 0 && y == 0 && z == 0;  }
	inline Vec3<t>	operator^(const Vec3<t>& v) const { return Vec3<t>().product(*this, v); }
	inline Vec3<t>	operator+(const Vec3<t>& v) const { return Vec3<t>().addition(*this, v); }
	inline Vec3<t>	operator-(const Vec3<t>& v) const { return Vec3<t>().substract(*this, v); }
	inline Vec3<t>	operator*(float f) const { return Vec3<t>(*this).product(f); }
	inline t		operator*(const Vec3<t>& v) const { return Vec3<t>(*this).product(v);}
	inline bool refract(Vec3<t>& normal, float eta) {
//		float eta = 1. / matIOR; // eta = in_IOR/out_IOR
		float cos_theta = -(*this * normal);
		if(cos_theta < 0.) {
			cos_theta *= -1 ;
			normal.product(-1);
			eta = 1. / eta;
		  }
		return refract_(normal, cos_theta, eta);
	}
	inline bool refract_(const Vec3<t>& normal, float cos_theta, float eta) {
		float k = 1. - eta * eta * (1. - cos_theta * cos_theta);
		if(k > 0) {
			this->addition( this->product(eta), normal * (eta * cos_theta - std::sqrt(k)) ).normalize();
		}
		return (k > 0);
	}
	inline void cartesian2sphericalDirection(float& phi, float& theta) const {
		phi = std::atan2(y, x);
		theta = std::acos(z / std::sqrt(x * x + y * y + z * z));
	}
	Vec3<t>& sphericalDirection2cartesian(float phi, float theta) {
		float sinTheta = std::sin(theta);
		x = sinTheta * std::cos(phi);
		y = sinTheta * std::sin(phi);
		z = std::cos(theta);
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
	Vec3<t>& randomInUnitSphere(void) {
		do {
			x = random_double() * 2.0 - 1.0;
			y = random_double() * 2.0 - 1.0;
			z = random_double() * 2.0 - 1.0;
		} while (sqnorm() >= 1.0);
		return *this;
	}
	Vec3<t>& randomInUnitHemisphere(const Vec3<t>& normal) {
		addition(randomInUnitSphere(), normal).normalize();
		return *this;
	}
	Vec3<t>& randomInUnitHemisphereCosineWeighted(const LookatAux& aux) {
		float phi = random_double() * M_2PI;
		float theta = std::acos(std::sqrt(random_double()));
		sphericalDirection2cartesian(phi, theta).lookatDir(aux);
		return *this;
	}
	Vec3<t>& randomInSphere(t r) {
		randomInUnitSphere().product(r);
		return *this;
	}
	Vec3<t>& toRt(int width, int height) { x -= width / 2; y = height / 2 - y - 1; return *this; }
	Vec3<t>& turnAroundX(float angle) {
		if ( !(angle == 0 || (y == 0 && z == 0)) ) {
			float sin = std::sin(angle), cos = std::cos(angle);
			float _z = z * cos - y * sin;
			float _y = z * sin + y * cos;
			y = _y; z = _z;
		}
		return *this;
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
	Vec3<t>& turnAroundZ(float angle) {
		if ( !(angle == 0 || (x == 0 && y == 0)) ) {
			float sin = std::sin(angle), cos = std::cos(angle);
			float _x = x * cos - y * sin;
			float _y = x * sin + y * cos;
			x = _x; y = _y;
		}
		return *this;
	}
	inline Vec3<t> turnAround_X(float angle) {
		return Vec3<t>(*this).turnAroundX(angle);
	}
	inline Vec3<t> turnAround_Y(float angle) {
		return Vec3<t>(*this).turnAroundY(angle);
	}
	inline Vec3<t> turnAround_Z(float angle) {
		return Vec3<t>(*this).turnAroundZ(angle);
	}
	std::string roundedOutput(int factor = 2) const {
		std::ostringstream o;
		o	<< "("
			<< std::setw(factor + 4) << std::right << std::setfill(' ')
			<< roundedString(x, factor) << ","
			<< std::setw(factor + 4) << std::right << std::setfill(' ')
			<< roundedString(y, factor) << ","
			<< std::setw(factor + 4) << std::right << std::setfill(' ')
			<< roundedString(z, factor) << ")";
		return o.str();
	}
	template <class > friend std::ostream& operator<<(std::ostream& s, const Vec3<t>& v);
	template <class > friend std::istringstream& operator>>(std::istringstream& is, Vec3<t>& v);
	template <class > friend bool operator==(const Vec3<t>& left, const Vec3<t>& right);
	template <class > friend bool operator!=(const Vec3<t>& left, const Vec3<t>& right);
};
template <class t> std::ostream& operator<<(std::ostream& s, const Vec3<t>& v) {
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
template <class t> bool operator==(const Vec3<t>& left, const Vec3<t>& right) {
	return left.x == right.x && left.y == right.y && left.z == right.z;
}
template <class t> bool operator!=(const Vec3<t>& left, const Vec3<t>& right) {
	return !(left == right);
}
// struct Vec3 end


// struct LookatAuxiliary
template <class t> struct LookatAuxiliary {
	Vec3<t> dir;
	Vec3<t> up;
	Vec3<t> right;
	LookatAuxiliary(const Vec3<t>& eyeDir) : dir(eyeDir), up(), right(0,-1,0) {
		if ( dir.x == 0 && (dir.y == -1 || dir.y == 1) && dir.z == 0) {
			right.y = 0; right.z = 1;
		}
		right.product(dir,right).normalize();
		up.product(dir,right).normalize();
	}
	~LookatAuxiliary(void) {}
};


// struct Position
struct Position {
	Vec3f	p;
	Vec3f	n;
	Position(void);
	~Position(void);
	Position(const Vec3f& point, const Vec3f& norm);
	Position(int px, int py, int pz, int nx, int ny, int nz);
	Position(const Position& other);
	Position& operator=(const Position& other);
	Position& lookat(const Position& eye, float roll = 0);
	Position& lookat(const Position& eye, const LookatAux& aux, float roll = 0);
	Position& roll(float roll);
};
std::ostream& operator<<(std::ostream& o, const Position& pos);
// struct Position end


// struct Probability
struct Probability : public Vec3f {
	Probability(void) : Vec3f() {}
	Probability(float refl, float refr, float diff, float max = 1) : Vec3f() {
		raw[0] = refl * max;
		raw[1] = raw[0] + refr * max;
		raw[2] = raw[1] + diff * max;
	}
	~Probability(void) {}
	Probability operator*=(float f) {
		for (int i = 0; i < 3; i++)
			raw[i] *= f;
		return *this;
	}
	inline float refl(void) const { return raw[0]; }
	inline float refr(void) const { return raw[1]; }
	inline float diff(void) const { return raw[2]; }
	inline bool  isDiffusion(void) const { return raw[2] != raw[1]; }

};
// struct Probability end


// Intersections, normals, rays
bool  raySphereIntersection(const Vec3f& rayDir, const Vec3f& rayPov, const Vec3f& center, float sqrRadius, float& distance, float& min_t, float& max_t, Hit& rayHit);
bool  raySphereIntersection(const Vec3f& rayDir, const Vec3f& k, float c, float& distance, float& min_t, float& max_t, Hit& rayHit);
void  normalToRaySphereIntersect(const Vec3f& intersectPt, const Vec3f& center, Vec3f& normal);
bool  rayPlaneIntersection(const Vec3f& rayDir, const Vec3f& rayPov, const Vec3f& point, const Vec3f& normal, float& distance, float& min_t, float& max_t, Hit& rayHit);
bool  rayPlaneIntersection(float k, const Vec3f r, const Vec3f& normal, float& distance, float& min_t, float& max_t, Hit& rayHit);

#endif
