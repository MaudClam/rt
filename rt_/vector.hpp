#ifndef VECTOR_HPP
# define VECTOR_HPP

# include <iomanip>
# include <sstream>
# include <random>


template <class t> struct	Vec2;
template <class t> struct	Vec3;
template <class t> class	Onb;
template <class t> struct	Pos;

typedef Vec2<double>	Vec2d;
typedef Vec2<float>		Vec2f;
typedef Vec2<int>		Vec2i;
typedef Vec3<double>	Vec3d;
typedef Vec3<float>		Vec3f;
typedef Vec3<int>		Vec3i;
typedef Onb<double>		Onb_d;
typedef Onb<float>		Onb_f;
typedef Onb<int>		Onb_i;
typedef Pos<double>		Pos_d;
typedef Pos<float>		Pos_f;
typedef Pos<int>		Pos_i;



template <class t> struct Vec2 {
	t	x, y;
	Vec2(void) : x(0), y(0) {}
	Vec2(t _x, t _y) : x(_x), y(_y) {}
	Vec2(const Vec2& other) : x(other.x), y(other.y) {}
	~Vec2(void) {}
	Vec2<t>& operator=(const Vec2& other) {
		if (this != &other) {
			x = other.x;
			y = other.y;
		}
		return *this;
	}
	Vec2<t>& operator+=(const Vec2<t>& other) { x + other.x, y + other.y; }
	Vec2<t>& operator-=(const Vec2<t>& other) { x - other.x, y - other.y; }
	Vec2<t>& operator*=(double d) { d * x, d * y; }
	Vec2<t>& operator*=(float f) { f * x, f * y; }
	Vec2<t>& operator*=(int n) { n * x, n * y; }
	Vec2<t>& set(t _x, t _y) { x = _x; y = _y; return *this; }
	Vec2<t>& deNaN(void) { x = deNaN(x); y = deNan(y); return * this; }
	Vec2<t>& cartsn2scan(t width, t height) { x += width / 2; y = height / 2 - y; return *this; }
	Vec2<t>& scan2cartsn(t width, t height) { x -= width / 2; y = height / 2 - y; return *this; }
	Vec2<t>& line2scan(int width, size_t ln) { x = int(ln) % width; y = int(ln) / width; return *this;}
	template <class > friend std::istringstream& operator>>(std::istringstream& is, Vec2<t>& v);

	inline Vec2<t> operator+(const Vec2<t>& other) const { return Vec2<t>(*this) += other; }
	inline Vec2<t> operator-(const Vec2<t>& other) const { return Vec2<t>(*this) -= other; }
	inline Vec2<t> operator*(t d) const { return Vec2<t>(*this) *= d; }
	inline bool isNull(void) const { return x == 0 && y == 0; }
	inline size_t scan2line(int width) const { return y * width + x; }
	template <class > friend std::ostream& operator<<(std::ostream& o, const Vec2<t>& v);
	template <class > friend bool operator==(const Vec2<t>& left, const Vec2<t>& right);
	template <class > friend bool operator!=(const Vec2<t>& left, const Vec2<t>& right);
};
template <class t> std::ostream& operator<<(std::ostream& os, const Vec2<t>& v) {
	os << (v.x == int(v.x) ? int(v.x) : v.x) << "," << (v.y == int(v.y) ? int(v.y) : v.y);
	return os;
}
template <class t> std::istringstream& operator>>(std::istringstream& is, Vec2<t>& v) {
	char trash;
	v.set(0,0);
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


template <class t> struct Vec3 {
	union {
		struct { t	x, y, z; };
		t raw[3];
	};
	Vec3(void) : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
	Vec3(const Vec3<t>& other) : x(other.x), y(other.y), z(other.z) {}
	~Vec3(void) {}
	Vec3<t>& operator=(const Vec3<t>& other) {
		if (this != &other) {
			for (int i = 0; i < 3; i++)
				raw[i] = other.raw[i];
		}
		return *this;
	}
	Vec3<t>& operator+=(const Vec3<t>& other) {
		for (int i = 0; i < 3; i++)
			raw[i] += other.raw[i];
		return *this;
	}
	Vec3<t>& operator-=(const Vec3<t>& other) {
		for (int i = 0; i < 3; i++)
			raw[i] -= other.raw[i];
		return *this;
	}
	Vec3<t>& operator^=(const Vec3<t>& other) { return *this = *this ^ other; }
	Vec3<t>& operator*=(t d) {
		for (int i = 0; i < 3; i++)
			raw[i] = d * raw[i];
		return *this;
	}
	Vec3<t>& set(t _x, t _y, t _z) { x = _x; y = _y; z = _z; return *this; }
	Vec3<t>& deNaN(void) { x = deNaN(x); y = deNan(y);  z = deNan(z); return * this; }
	Vec3<t>& normalize(t l = 1) {
		t _norm = norm();
		if (_norm != 0) {
			*this *= (l / _norm);
		}
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
	Vec3<t>& sphercl2cartsn(t phi, t theta, t r = 1) {
		float sinTheta = std::sin(theta);
		x = sinTheta * std::cos(phi);
		y = sinTheta * std::sin(phi);
		z = std::cos(theta);
		if (r != 1)
			*this *= r;
		return *this;
	}
	Vec3<t>& toLocalDirection(const Onb<t>& onb) { onb.dir2local(*this); return *this; }
	Vec3<t>& toGlobalDirection(const Onb<t>& onb) { onb.dir2global(*this); return *this; }

	
	Vec3<t>& toLocalDirection(const Pos<t>& pos) { pos.dir2local(*this); return *this; }
	Vec3<t>& toLocalPoint(const Pos<t>& pos) { pos.point2local(*this); return *this; }
	Vec3<t>& toRandomDirection(void) {
		do {
			x = randomUnitCoordinate();
			y = randomUnitCoordinate();
			z = randomUnitCoordinate();
		} while (sqnorm() >= 1.0f);
		normalize();
		return *this;
	}
	Vec3<t>& toRandomDirectionInHemisphere(const Vec3<t>& normal) {
		*this = toRandomDirection() + normal;
		normalize();
		return *this;
	}
	Vec3<t>& toRandomCosineDirection(void) {
		float phi = random_double() * M_2PI;
		float theta = std::acos(std::sqrt(random_double()));
		sphercl2cartsn(phi, theta);
		return *this;
	}
	Vec3<t>& toRandomCosineDirection(const Onb<t>& onb) {
		toRandomCosineDirection();
		toLocalDirection(onb);
		return *this;
	}
	Vec3<t>& toRandomCosineDirection(const Vec3<t>& normal) {
		randomCosineDirection(Onb<t>(normal));
		return *this;
	}
	Vec3<t>& reflect(const Vec3<t>& norm) {
		*this *= -1;
		norm * (*this * norm * 2) - *this;
		normalize();
		return *this;
	}
	inline bool refract(const Vec3<t>& normal, float eta) {
		float cos_theta = -(*this * normal);
		if(cos_theta < 0.) {
			cos_theta *= -1 ;
			eta = 1. / eta;
			return refract_(normal * -1, cos_theta, eta);
		  }
		return refract_(normal, cos_theta, eta);
	}
	inline bool refract_(const Vec3<t>& normal, float cos_theta, float eta) {
		float k = 1. - eta * eta * (1. - cos_theta * cos_theta);
		if(k > 0) {
			*this = (*this *= eta) + normal * (eta * cos_theta - std::sqrt(k));
			normalize();
		}
		return (k > 0);
	}

	inline Vec3<t> operator+(const Vec3<t>& other) const { return Vec3<t>(*this) += other; }
	inline Vec3<t> operator-(const Vec3<t>& other) const { return Vec3<t>(*this) -= other; }
	inline Vec3<t> operator^(const Vec3<t>& other) const {
		return Vec3<t>(raw[1] * other.raw[2] - raw[2] * other.raw[1],
					   raw[2] * other.raw[0] - raw[0] * other.raw[2],
					   raw[0] * other.raw[1] - raw[1] * other.raw[0]);
	}
	inline Vec3<t> operator*(t d) const { return Vec3<t>(*this) *= d; }
	inline Vec3<t> localDirection(const Onb<t>& onb) const { return Vec3<t>(*this).toLocalDirection(onb); }
	inline Vec3<t> localDirection(const Pos<t>& pos) const { return Vec3<t>(*this).toLocalDirection(pos); }
	inline Vec3<t> local(const Pos<t>& pos) const { return Vec3<t>(*this).toLocal(pos); }
	inline t       sqnorm(void) const { return x * x + y * y + z * z; }
	inline t       norm(void) const { return std::sqrt(sqnorm()); }
	inline bool    isNull(void) const { return x == 0 && y == 0 && z == 0; }
	inline void    cartsn2sphercl(float& phi, float& theta, float& r) const {
		r = std::sqrt(x * x + y * y + z * z);
		phi = std::atan2(y, x);
		theta = std::acos(z / r);
	}
	template <class > friend std::ostream& operator<<(std::ostream& os, const Vec3<t>& v);
	template <class > friend std::istringstream& operator>>(std::istringstream& is, Vec3<t>& v);
	template <class > friend bool operator==(const Vec3<t>& left, const Vec3<t>& right);
	template <class > friend bool operator!=(const Vec3<t>& left, const Vec3<t>& right);
};
template <class t> std::ostream& operator<<(std::ostream& os, const Vec3<t>& v) {
	os << (v.x == int(v.x) ? int(v.x) : v.x) << ",";
	os << (v.y == int(v.y) ? int(v.y) : v.y) << ",";
	os << (v.z == int(v.z) ? int(v.z) : v.z);
	return os;
}
template <class t> std::istringstream& operator>>(std::istringstream& is, Vec3<t>& v) {
	char trash;
	v.set(0,0,0);
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


template <class t> class Onb {	/*Ortho-Normal Basis*/
	union {
		struct { Vec3<t> _u, _v, _w; };
		Vec3<t> axis[3];
	};
	Onb<t>& _build_from_w(const Vec3<t>& w) {
		_w = w;
		_w.normalize();
		if (_w.x == 0 && std::abs(_w.y) == 1 && _w.z == 0)
			_u.set(0, 0, 1);
		else
			_u.set(0, -1, 0);
		_u = (_w ^ _u);
		_v = (_w ^ _u).normalize();
		return *this;
	}
public:
	Onb(void) : _u(), _v(), _w() { this->_set_global(); }
	Onb(const Vec3<t>& w) : _u(), _v(), _w() { _build_from_w(w); }
	Onb(const Onb<t>& other) : _u(other._u), _v(other._v), _w(other._w) {}
	~Onb(void) {}
	Onb<t>& operator=(const Onb<t>& other) {
		if (this != &other) {
			_u = other._u;
			_v = other._v;
			_w = other._w;
		}
		return *this;
	}
	Onb<t>& set_w(const Vec3<t>& w) { _build_from_w(w); return *this; }
	Onb<t>& turnAroundU(t angle) {
		_v.turnAroundX(angle);
		_w.turnAroundX(angle);
		return *this;
	}
	Onb<t>& turnAroundV(t angle) {
		_u.turnAroundY(angle);
		_w.turnAroundY(angle);
		return *this;
	}
	Onb<t>& turnAroundW(t angle) {
		_u.turnAroundZ(angle);
		_v.turnAroundZ(angle);
		return *this;
	}
	inline Vec3<t> u(void) const { return _u; }
	inline Vec3<t> v(void) const { return _v; }
	inline Vec3<t> w(void) const { return _w; }
	inline void dir2local(Vec3<t>& dir) const {
		dir.x = dir.x * _u.x + dir.y * _u.y + dir.z * _u.z; // Проекция на u
		dir.y = dir.x * _v.x + dir.y * _v.y + dir.z * _v.z; // Проекция на v
		dir.z = dir.x * _w.x + dir.y * _w.y + dir.z * _w.z; // Проекция на w
	}
	inline void dir2global(Vec3<t>& dir) const {
		dir.x = dir.x * _u.x + dir.y * _v.x + dir.z * _w.x; // Проекция на глобальные оси
		dir.y = dir.x * _u.y + dir.y * _v.y + dir.z * _w.y;
		dir.z = dir.x * _u.z + dir.y * _v.z + dir.z * _w.z;
	}
	std::string output(void) {
		std::ostringstream os;
		os << "(" << _u << "),(" << _v << "),(" << _w << ")";
		return os.str();
	}
	template <class > friend std::ostream& operator<<(std::ostream& os, const Onb<t>& onb);
	template <class > friend std::istringstream& operator>>(std::istringstream& is, Onb<t>& onb);
};
template <class t> std::ostream& operator<<(std::ostream& os, const Onb<t>& onb) {
	os << onb.w();
	return os;
}
template <class t> std::istringstream& operator>>(std::istringstream& is, Onb<t>& onb) {
	onb.set_w((is >> Vec3<t>()));
	return is;
}


template <class t> struct Pos : public Vec3<t> {
	Onb<t> onb;
	Pos(void) : Vec3<t>(), onb() {}
	Pos(const Pos& other) : Vec3<t>(), onb() { *this == other; }
	Pos(const Vec3<t>& o, const Vec3<t>& w)  : Vec3<t>(o), onb(w)  {}
	~Pos(void) {}
	Pos<t>& operator=(const Pos<t>& other) {
		if (this != &other) {
			for (int i = 0; i < 3; i++)
				this->raw[i] = other.raw[i];
			this->onb = other.onb;
		}
		return *this;
	}
	Pos<t>& turnAroundU(t angle) { onb.turnAroundU(angle); return *this; }
	Pos<t>& turnAroundV(t angle) { onb.turnAroundV(angle); return *this; }
	Pos<t>& turnAroundW(t angle) { onb.turnAroundW(angle); return *this; }

	Pos<t>& toGlobal(void) {
		*this += ( (onb._u *= this->x) + (onb._v *= this->y) + (onb._w *= this->z) );
		onb._set_global();
		return *this;
	}
	inline Vec3<t> global(void) const {
		return *this + (onb._u * this->x) + (onb._v * this->y) + (onb._w * this->z);
	}
	inline Pos<t>& move(const Vec3<t>& m) {
		*this += (onb.u() * m.x + onb.v() * m.y + onb.w() * m.z);
		return *this;
	}
	
	
	
	inline void point2local(Vec3<t>& point) const {
		point = *this - point;
	}
	inline void dir2local(Vec3<t>& dir) const {
		dir = onb.dir2local(dir);
	}
};

#endif


//// Преобразование из глобальной в локальную систему координат
//Vector3 globalToLocal(const Vector3& global, const Vector3& u, const Vector3& v, const Vector3& w) {
//	Vector3 local;
//	dir.x = dir.x * _u.x + dir.y * _u.y + dir.z * _u.z; // Проекция на u
//	dir.y = dir.x * _v.x + dir.y * _v.y + dir.z * _v.z; // Проекция на v
//	dir.z = dir.x * _w.x + dir.y * _w.y + dir.z * _w.z; // Проекция на w
//	return local;
//}
//
//// Преобразование из локальной в глобальную систему координат
//Vector3 localToGlobal(const Vector3& local, const Vector3& u, const Vector3& v, const Vector3& w) {
//	Vector3 global;
//	dir.x = dir.x * _u.x + dir.y * _v.x + dir.z * _w.x; // Проекция на глобальные оси
//	dir.y = dir.x * _u.y + dir.y * _v.y + dir.z * _w.y;
//	dir.z = dir.x * _u.z + dir.y * _v.z + dir.z * _w.z;
//	return global;
