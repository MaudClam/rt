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
typedef std::vector<int>		texture2_t;

const float M_2PI(2.0 * M_PI);
const float M_PI_180(M_PI / 180.);
const float M_180_PI(180. / M_PI);
const float _2INFINITY(2. * _INFINITY);
const float SQ_OUTLINE_WIDTH(OUTLINE_WIDTH * OUTLINE_WIDTH);
const float M_1_2PI(1. / M_2PI);

enum Hit { FRONT, BACK, OUTLINE, ANY_SHADOW, ALL_SHADOWS, INSIDE, OUTSIDE, IN_VOLUME };
enum CombineType { END=0, UNION, SUBTRACTION, INTERSECTION };
enum MapType {NO, CAUSTIC, GLOBAL, VOLUME, RESET};
enum TracingType { RAY, PATH };


// Non class functions
int		i2limits(int num, int min, int max);
float	f2limits(float num, float min, float max);
double	random_double(void);
double	randomUnitCoordinate(void);
float	randomCoordinate(float n);
bool	almostEqual(float a, float b, float precision = PRECISION);
float	degree2radian(float degree);
float	radian2degree(float radian);
bool	rayHitDefinition(float& min_t, float& max_t, float& distance, Hit& rayHit);
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
	Vec2<t>& string2scan(int width, size_t ln) { x = int(ln) % width; y = int(ln) / width; return *this;}
	Vec2<t>& scan2cartesian(t width, t height) { x -= width / 2; y = height / 2 - y; return *this; }
	Vec2<t>& cartesian2scan(t width, t height) { x += width / 2; y = height / 2 - y; return *this; }
	inline size_t scan2string(int width) const { return y * width + x; }
	inline bool isNull(void) { return x == 0 && y == 0; }
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
	inline bool  isNull(void) const { return x == 0 && y == 0 && z == 0; }
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
	inline void cartesian2spherical(float& phi, float& theta, float& r) const {
		r = std::sqrt(x * x + y * y + z * z);
		phi = std::atan2(y, x);
		theta = std::acos(z / r);
	}
	Vec3<t>& spherical2cartesian(float phi, float theta, float r = 1) {
		float sinTheta = std::sin(theta);
		x = sinTheta * std::cos(phi);
		y = sinTheta * std::sin(phi);
		z = std::cos(theta);
		if (r != 1)
			product(r);
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
			x = randomUnitCoordinate();
			y = randomUnitCoordinate();
			z = randomUnitCoordinate();
		} while (sqnorm() >= 1.0f);
		normalize();
		return *this;
	}
	Vec3<t>& randomInUnitHemisphere(const Vec3<t>& normal) {
		addition(randomInUnitSphere(), normal).normalize();
		return *this;
	}
	Vec3<t>& randomInUnitHemisphereCosineDistribution(const LookatAux& aux) {
		float phi = random_double() * M_2PI;
		float theta = std::acos(std::sqrt(random_double()));
		spherical2cartesian(phi, theta).lookatDir(aux);
		return *this;
	}
	Vec3<t>& randomInUnitHemisphereCosineDistribution(const Vec3f& normal) {
		LookatAuxiliary<t> aux(normal);
		return randomInUnitHemisphereCosineDistribution(aux);
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
	s << std::setw(6) << std::right << std::setfill(' ') << os.str();
	s << std::setw(9) << std::left << std::setfill(' ') << os1.str();
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


// class Texture2
class Texture2 : public texture2_t {
	std::string _id;
	int _width, _height;
public:
	Texture2(void);
	Texture2(const Texture2& other);
	Texture2& operator=(const Texture2& other);
	~Texture2(void);
	inline std::string get_id(void) const { return _id; }
	inline int get_width(void) const { return _width; }
	inline int get_height(void) const { return _height; }
	inline int get_rgba(const Vec2f& p) const { return get_rgba(p.u, p.v); }
	inline int get_rgba(float u, float v) const {
		size_t i = Vec2i(u * _width, v * _height).scan2string(_width);
		if (DEBUG && i >= size()) std::cout << i << " ";//FIXME
		i = i < size() ? i : 0;
		return (*this)[i];
	}
	inline void set_id(const std::string& id) { _id = id; }
	inline void set_width(int width) { _width = std::abs(width); _height = int(size()) / _width; }
};

// struct A_Planar
struct A_Planar {
	Position	pos;	// plane position
	Vec3f		u;		// plane X-axis
	Vec3f		v;		// plane Y-axis
	float		angle;	// plane axes tilt
	Vec2f		ratio;
	Texture2*	txtr;
	A_Planar(void);
	A_Planar(Texture2* _txtr);
	A_Planar(const A_Planar& other);
	virtual ~A_Planar(void);
	A_Planar& operator=(const A_Planar& other);
	inline void  lookat(const Position& eye, const LookatAux& aux, float roll) {
		if (roll == 0) {
			pos.n.lookatDir(aux);
			pos.p.lookatPt(eye.p, aux);
			u.lookatDir(aux);
			v.lookatDir(aux);
		} else {
			this->roll(-roll);
			pos.n.lookatDir(aux);
			pos.p.lookatPt(eye.p, aux);
			u.lookatDir(aux);
			v.lookatDir(aux);
			this->roll(roll);
		}
	}
	inline void  roll(float roll) {
		pos.p.turnAroundZ(roll);
		pos.n.turnAroundZ(roll);
		u.turnAroundZ(roll);
		v.turnAroundZ(roll);
	}
	inline bool  isTexture(void) const { return txtr; }
	inline void  set_planeGeometry(std::istringstream& is) {
		is >> pos.p >> pos.n >> angle;
		pos.n.normalize();
		angle = degree2radian(f2limits(angle, 0, 360));
		if (std::max(std::abs(pos.n.x), std::max(std::abs(pos.n.y), std::abs(pos.n.z))) == std::abs(pos.n.y)) {
			u.set_xyz(1,0,0); v.set_xyz(0,0,-1);
			u.turnAroundY(angle);
			v.turnAroundY(angle);
		} else if (std::max(std::abs(pos.n.x), std::max(std::abs(pos.n.y), std::abs(pos.n.z))) == std::abs(pos.n.x)) {
			u.set_xyz(0,0,1); v.set_xyz(0,1,0);
			u.turnAroundX(angle);
			v.turnAroundX(angle);
		} else {
			u.set_xyz(1,0,0); v.set_xyz(0,1,0);
			u.turnAroundZ(angle);
			v.turnAroundZ(angle);
		}
	}
	inline bool  planeIntersection(const Vec3f& rayPov, float normСos, float& distance) const {
		distance = (pos.n * pos.p - pos.n * rayPov) / normСos;
		return distance >= 0;
	}
	inline bool  intersection(const Vec3f& rayPov, const Vec3f& rayDir, float& distance, float& min_t, float& max_t, Hit& hit) const {
		float normСos = rayDir * pos.n;
		if (normСos != 0) {
			planeIntersection(rayPov, normСos, distance);
			if (normСos < 0) {
				max_t = _2INFINITY;
				min_t = distance;
			} else {
				if (distance < 0)
					return false;
				max_t = distance;
				min_t = -_2INFINITY;
			}
			if (figureIntersection(localHitPoint(rayPov, rayDir, distance), hit)) {
				return rayHitDefinition(min_t, max_t, distance, hit);
			}
		}
		return false;
	};
	inline Vec3f localHitPoint(const Vec3f& rayPov, const Vec3f& rayDir, float distance) const {
		return rayPov + (rayDir * distance) -  pos.p;
	}
	inline Vec3f localHitPoint(const Vec3f& recPov) const {
		return recPov -  pos.p;
	}
	inline int   getPlaneTextureRgba(const Vec3f& localHitPoint, float width, float height) const {
		Vec2f p(localHitPoint * u + _2INFINITY, localHitPoint * v - _2INFINITY);
		p.cartesian2scan(width, height);
		p.u = std::fmod(p.u * ratio.u, width) / width;
		p.v = std::fmod(p.v * ratio.v, height) / height;
		return txtr->get_rgba(p);
	}
	inline std::string output_planeGeometry(void) const {
		std::ostringstream os;
		os << " " << std::setw(5) << std::right << pos.p;
		os << " " << std::setw(5) << std::right << pos.n;
		os << " " << std::setw(5) << std::right << roundedString(radian2degree(angle), 1);
		return os.str();
	}
	inline std::string getTextureName_if(void) {
		if (txtr)
			return txtr->get_id();
		return "";
	}

	virtual inline A_Planar* clone(void) const = 0;
	virtual inline void  set_geometry(std::istringstream& is) = 0;
	virtual inline float area(void) const = 0;
	virtual inline bool  figureIntersection(const Vec3f& localHitPoint, Hit& hit) const = 0;
	virtual inline float getMaxSize(void) const = 0;
	virtual inline Vec3f getRandomPoint(void) const = 0;
	virtual inline int   getTextureRgba(const Vec3f& localHitPoint) const = 0;
	virtual inline std::string output_geometry(void) const = 0;
};

// struct Plane
struct Plane : public A_Planar {
	Plane(Texture2* _txtr);
	~Plane(void);
	inline Plane* clone(void) const {
		Plane* plane = new Plane(*this);
		return plane;
	}
	inline void  set_geometry(std::istringstream& is) { set_planeGeometry(is); }
	inline float area(void) const { return _INFINITY * _INFINITY; }
	inline bool  figureIntersection(const Vec3f& localHitPoint, Hit& hit) const {
		(void)localHitPoint; (void)hit;
		return true;
	}
	inline float getMaxSize(void) const { return _INFINITY; }
	inline Vec3f getRandomPoint(void) const {
		return pos.p + (u * randomCoordinate(_INFINITY)) + (v * randomCoordinate(_INFINITY));
	}
	inline int   getTextureRgba(const Vec3f& localHitPoint) const {
		return getPlaneTextureRgba(localHitPoint, 1, 1);
	}
	inline std::string output_geometry(void) const { return output_planeGeometry(); }
};

// struct Circle
struct Circle : public A_Planar {
	float r;
	float sqR;
	Circle(void);
	Circle(Texture2* _txtr);
	Circle(const Circle& other);
	~Circle(void);
	Circle& operator=(const Circle& other);
	inline Circle* clone(void) const {
		Circle* circle = new Circle(*this);
		return circle;
	}
	inline void  set_geometry(std::istringstream& is) {
		set_planeGeometry(is);
		is >> r;
		r = r < 0 ? 0 : r * 0.5;
		sqR = r * r;
	}
	inline float area(void) const { return sqR * M_PI; }
	inline bool  figureIntersection(const Vec3f& localHitPoint, Hit& hit) const {
		float sqD_r = sqR - localHitPoint.sqnorm();
		if (sqD_r >= 0) {
			if ( (std::abs(sqD_r) <= SQ_OUTLINE_WIDTH) )
				hit = OUTLINE;
			return true;
		}
		return false;
	}
	inline float getMaxSize(void) const { return r * 2; }
	inline Vec3f getRandomPoint(void) const {
		Vec3f point;
		do {
			point.x = randomUnitCoordinate();
			point.y = randomUnitCoordinate();
			point.z = 0;
		} while (point.sqnorm() >= 1.0f);
		point.product(r);
		return pos.p + (u * point.x) + (v * point.y);
	}
	inline int   getTextureRgba(const Vec3f& localHitPoint) const {
		return getPlaneTextureRgba(localHitPoint, r * 2, r * 2);
	}
	inline std::string output_geometry(void) const {
		std::ostringstream os;
		os << std::setw(18) << std::left << output_planeGeometry();
		os << " " << std::setw(5) << std::right << r * 2;
		return os.str();
	}
};

// struct Rectangle
struct Rectangle : public A_Planar {
	float w_2;
	float h_2;
	Rectangle(void);
	Rectangle(Texture2* _txtr);
	Rectangle(const Rectangle& other);
	~Rectangle(void);
	Rectangle& operator=(const Rectangle& other);
	inline Rectangle* clone(void) const {
		Rectangle* rectangle = new Rectangle(*this);
		return rectangle;
	}
	virtual inline void  set_geometry(std::istringstream& is) {
		set_planeGeometry(is);
		is >> w_2 >> h_2;
		w_2 = w_2 <= 0 ? 0 : w_2 * 0.5;
		h_2 = h_2 <= 0 ? 0 : h_2 * 0.5;
	}
	inline float area(void) const { return 4 * w_2 * h_2; }
	inline bool  figureIntersection(const Vec3f& localHitPoint, Hit& hit) const {
		float d_u = w_2 - std::abs(localHitPoint * u);
		float d_v = h_2 - std::abs(localHitPoint * v);
		if (d_u >= 0 && d_v >= 0) {
			if ( std::abs(d_u) <= OUTLINE_WIDTH && std::abs(d_v) <= OUTLINE_WIDTH )
				hit = OUTLINE;
				return true;
		}
		return d_u >= 0 && d_v >= 0;
	}
	inline float getMaxSize(void) const { return 2.0f * std::sqrt(w_2 * w_2 + h_2 * h_2); }
	inline Vec3f getRandomPoint(void) const {
		return pos.p + (u * randomCoordinate(w_2)) + (v * randomCoordinate(h_2));
	}
	inline int   getTextureRgba(const Vec3f& localHitPoint) const {
		return getPlaneTextureRgba(localHitPoint, w_2 * 2, h_2 * 2);
	}
	virtual inline std::string output_geometry(void) const {
		std::ostringstream os;
		os << std::setw(18) << std::left << output_planeGeometry();
		os << " " << std::setw(5) << std::right << w_2 * 2;
		os << " " << std::setw(5) << std::right << h_2 * 2;
		return os.str();
	}
};

// struct Square
struct Square : public Rectangle {
	Square(void);
	Square(Texture2* _txtr);
	Square(const Square& other);
	~Square(void);
	inline void  set_geometry(std::istringstream& is) {
		set_planeGeometry(is);
		is >> w_2;
		w_2 = w_2 <= 0 ? 0 : w_2 * 0.5;
		h_2 = w_2;
	}
	inline std::string output_geometry(void) const {
		std::ostringstream os;
		os << std::setw(18) << std::left << output_planeGeometry();
		os << " " << std::setw(5) << std::right << w_2 * 2;
		return os.str();
	}
};

//	struct Sphere 2
struct Sphere2 {
	Position pos;
	float	r;
	float	sqR;
	Vec3f	u;
	Vec3f	v;
	float	angle;
	Vec2f	ratio;
	Texture2*	txtr;

	Sphere2(void);
	Sphere2(Texture2* _txtr);
	Sphere2(const Sphere2& other);
	Sphere2&	operator=(const Sphere2& other);
	~Sphere2(void);
	inline void  lookat(const Position& eye, const LookatAux& aux, float roll) {
		if (roll == 0) {
			pos.n.lookatDir(aux);
			pos.p.lookatPt(eye.p, aux);
			u.lookatDir(aux);
			v.lookatDir(aux);
		} else {
			this->roll(-roll);
			pos.n.lookatDir(aux);
			pos.p.lookatPt(eye.p, aux);
			u.lookatDir(aux);
			v.lookatDir(aux);
			this->roll(roll);
		}
	}
	inline void  roll(float roll) {
		pos.p.turnAroundZ(roll);
		pos.n.turnAroundZ(roll);
		u.turnAroundZ(roll);
		v.turnAroundZ(roll);
	}
	inline bool  isTexture(void) const { return txtr; }
	inline void  set_geometry(std::istringstream& is) {
		is >> pos.p >> pos.n >> angle >> r;
		pos.n.normalize();
		angle = degree2radian(f2limits(angle, 0, 360));
		r = r < 0 ? -r * 0.5 : r * 0.5;
		sqR = r * r;
		if (std::max(std::abs(pos.n.x), std::max(std::abs(pos.n.y), std::abs(pos.n.z))) == std::abs(pos.n.y)) {
			u.set_xyz(1,0,0); v.set_xyz(0,0,-1);
			u.turnAroundY(angle);
			v.turnAroundY(angle);
		} else if (std::max(std::abs(pos.n.x), std::max(std::abs(pos.n.y), std::abs(pos.n.z))) == std::abs(pos.n.x)) {
			u.set_xyz(0,0,1); v.set_xyz(0,1,0);
			u.turnAroundX(angle);
			v.turnAroundX(angle);
		} else {
			u.set_xyz(1,0,0); v.set_xyz(0,1,0);
			u.turnAroundZ(angle);
			v.turnAroundZ(angle);
		}
	}
	inline bool	intersection(const Vec3f& rayPov, const Vec3f& rayDir, float& distance, float& min_t, float& max_t, Hit& rayHit) const {
		Vec3f	k = rayPov - pos.p;
		float	c = k * k - sqR;
		float	b = rayDir * k;
		float	d = b * b - c;
		if (d >= 0) {
			float sqrt_d = std::sqrt(d);
			float t1 = -b + sqrt_d;
			float t2 = -b - sqrt_d;
			min_t = std::min(t1, t2);
			max_t = std::max(t1, t2);
			if (std::abs(d) <= OUTLINE_WIDTH)
				rayHit = OUTLINE;
			return (rayHitDefinition(min_t, max_t, distance, rayHit));
		}
		return false;
	}
	inline Vec3f localHitPoint(const Vec3f& rayPov, const Vec3f& rayDir, float distance) const {
		return rayPov + (rayDir * distance) -  pos.p;
	}
	inline Vec3f localHitPoint(const Vec3f& recPov) const {
		return recPov -  pos.p;
	}
	inline	Sphere2*	clone(void) const {
		return new Sphere2(*this);
	}
	inline int   getTextureRgba(const Vec3f& loc) const {
		float phi(0), theta(0), _r(0);
		Vec3f(loc * u, loc * v, loc * pos.n).cartesian2spherical(phi, theta, _r);
		phi = std::fmod((phi + M_PI) * ratio.u, M_2PI) / (M_2PI + PRECISION);
		theta = std::fmod(theta * ratio.v, M_PI) / (M_PI + PRECISION);
		return txtr->get_rgba(phi, theta);
	}
	inline Vec3f getRandomPoint(void) const {
		return Vec3f().randomInSphere(r) + pos.p;
	}
	inline std::string output_geometry(void) const {
		std::ostringstream os;
		os << " " << std::setw(5) << std::right << pos.p;
		os << " " << std::setw(5) << std::right << pos.n;
		os << " " << std::setw(5) << std::right << angle;
		os << " " << std::setw(5) << std::right << r * 2;
		return os.str();
	}
	inline void	getNormal(const Vec3f& intersectPt, Vec3f& normal) {
		normal.substract(intersectPt, pos.p).normalize();
	}
	inline std::string getTextureName_if(void) {
		if (txtr)
			return txtr->get_id();
		return "";
	}
};


// Intersections, normals, rays
bool raySphereIntersection(const Vec3f& rayDir, const Vec3f& rayPov, const Vec3f& center, float sqrRadius, float& distance, float& min_t, float& max_t, Hit& rayHit);
bool raySphereIntersection(const Vec3f& rayDir, const Vec3f& k, float c, float& distance, float& min_t, float& max_t, Hit& rayHit);
void normalToRaySphereIntersect(const Vec3f& intersectPt, const Vec3f& center, Vec3f& normal);
bool rayPlaneIntersection(const Vec3f& rayDir, const Vec3f& rayPov, const Vec3f& center, const Vec3f& normal, float& distance, float& min_t, float& max_t, Hit& rayHit);
#endif
