#include "geometry.hpp"


// Non class functions

int		i2limits(int num, int min, int max) {
	if (num < min) {
		return min;
	} else if (num > max) {
		return max;
	}
	return num;
}

float	f2limits(float num, float min, float max) {
	if (num < min) {
		return min;
	} else if (num > max) {
		return max;
	}
	return num;
}

double	random_double(void) {
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937				generator;
	static std::function<double()>	rand_generator = std::bind(distribution, generator);
	return rand_generator();
}

double	randomUnitCoordinate(void) {
	return (random_double() * 2.0 - 1.0);
}

float	randomCoordinate(float n) {
	return randomUnitCoordinate() * n;
}

bool	almostEqual(float a, float b, float precision) { return std::fabs(a - b) < precision; }

float	degree2radian(float degree) {
	return (degree * M_PI_180);
}

float	radian2degree(float radian) {
	return (radian * M_180_PI);
}

float	getShining(const Vec3f& dirFromPov, const Vec3f& normal, const Vec3f& dirToLight, float glossy) {
	return getShining_(dirFromPov, normal, dirToLight, glossy);
}

float	getShining_(const Vec3f& dirFromPov, const Vec3f& normal, Vec3f dirToLight, float glossy) {
	if (glossy) {
		float k = dirFromPov * dirToLight.reflect(normal);
		if (k > +0)
			return std::pow(k, glossy);
	}
	return 0;
}

float	getSchlick(float cosine, float eta) {
	float r0 = (1 - eta) / (1 + eta);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}

std::string roundedString(float num, int factor) {
	std::string sign("");
	if (num < 0) {
		sign = "-";
		num = -num;
	}
	if (factor > 0) {
		factor = std::pow(10, factor);
	} else {
		factor = 1;
	}
	num *= factor;
	int intgr = (int)std::round(num);
	int frctnl = intgr % factor;
	intgr /= factor;
	if (intgr == 0 && frctnl == 0) {
		sign = "";
	}
	return sign + std::to_string(intgr) + "." + std::to_string(frctnl);
}

std::string combineType(CombineType type) {
	switch (type) {
		case END:
			return "END";
		case UNION:
			return "UNION";
		case SUBTRACTION:
			return "SUBTRACTION";
		case INTERSECTION:
			return "INTERSECTION";
	}
	return "";
}

std::string mapType(MapType type) {
	switch (type) {
		case NO:
			return "NO";
		case CAUSTIC:
			return "CAUSTIC";
		case GLOBAL:
			return "GLOBAL";
		case VOLUME:
			return "VOLUME";
		case RESET:
			return "RESET";
	}
	return "";
}


// Struct Position

Position::Position(void) : p(), n() {}

Position::~Position(void) {}

Position::Position(const Vec3f& point, const Vec3f& norm) : p(point), n(norm) { n.normalize(); }

Position::Position(int px, int py, int pz, int nx, int ny, int nz) : p(px,py,pz), n(nx,ny,nz) { n.normalize(); }

Position::Position(const Position& other) : p(other.p), n(other.n) {}

Position& Position::operator=(const Position& other) {
	if (this != &other) {
		p = other.p;
		n = other.n;
	}
	return *this;
};

Position& Position::lookat(const Position& eye, float roll) {
	LookatAux aux(eye.n);
	lookat(eye, aux, roll);
	return *this;
}

Position& Position::lookat(const Position& eye, const LookatAux& aux, float roll) {
	if (roll == 0) {
		n.lookatDir(aux);
		p.lookatPt(eye.p, aux);
	} else {
		this->roll(-roll);
		n.lookatDir(aux);
		p.lookatPt(eye.p, aux);
		this->roll(roll);
	}
	return *this;
}

Position& Position::roll(float roll) {
	p.turnAroundZ(roll);
	n.turnAroundZ(roll);
	return *this;
}


std::ostream& operator<<(std::ostream& o, const Position& pos) {
	o << "(" << pos.p.x << "," << pos.p.y << "," << pos.p.z << "),";
	o << "(" << pos.n.x << "," << pos.n.y << "," << pos.n.z << ")";
	return o;
}

// class Texture2

Texture2::Texture2(void) : texture2_t(), _id(), _width(0), _height(0) {}

Texture2::Texture2(const Texture2& other) : texture2_t(), _id(), _width(0), _height(0) { *this = other; }

Texture2& Texture2::operator=(const Texture2& other) {
	if (this != &other) {
		clear();
		insert(begin(), other.begin(), other.end());
		set_id(other._id);
		set_width(other._width);
	}
	return *this;
}

Texture2::~Texture2(void) {}


// struct A_Planar

A_Planar::A_Planar(void) :
pos(),
u(),
v(),
angle(0),
txtr(NULL)
{ u.x = 1; v.y = 1; pos.n.z = 1; }

A_Planar::A_Planar(Texture2* _txtr) :
pos(),
u(),
v(),
angle(0),
txtr(_txtr)
{ u.x = 1; v.y = 1; pos.n.z = 1; }

A_Planar::A_Planar(const A_Planar& other) : A_Planar() { *this = other; }

A_Planar::~A_Planar(void) {}

A_Planar& A_Planar::operator=(const A_Planar& other) {
	if (this != &other) {
		pos = other.pos;
		u = other.u;
		v = other.v;
		angle = other.angle;
		txtr = other.txtr;
	}
	return *this;
}


// struct Plane

Plane::Plane(Texture2* _txtr) : A_Planar(_txtr) {}

Plane::~Plane(void) {}


// struct Сircle

Сircle::Сircle(void) : A_Planar(), r(0), sqR(0) {}

Сircle::Сircle(Texture2* _txtr) : A_Planar(_txtr), r(0), sqR(0) {}

Сircle::Сircle(const Сircle& other) : A_Planar(), r(0), sqR(0) { *this = other; }

Сircle::~Сircle(void) {}

Сircle& Сircle::operator=(const Сircle& other) {
	if (this != &other) {
		pos = other.pos;
		u = other.u;
		v = other.v;
		angle = other.angle;
		txtr = other.txtr;
		r = other.r;
		sqR = other.sqR;
	}
	return *this;
}


// struct Rectangle

Rectangle::Rectangle(void) : A_Planar(), w_2(0), h_2(0) {}

Rectangle::Rectangle(Texture2* _txtr) : A_Planar(_txtr), w_2(0), h_2(0) {}

Rectangle::Rectangle(const Rectangle& other) : A_Planar(), w_2(0), h_2(0) { *this = other; }

Rectangle::~Rectangle(void) {}

Rectangle& Rectangle::operator=(const Rectangle& other) {
	if (this != &other) {
		pos = other.pos;
		u = other.u;
		v = other.v;
		angle = other.angle;
		txtr = other.txtr;
		w_2 = other.w_2;
		h_2 = other.h_2;
	}
	return *this;
}


// Intersections, normals, rays

bool	raySphereIntersection(const Vec3f& rayDir,
							  const Vec3f& rayPov,
							  const Vec3f& center,
							  float sqrRadius,
							  float& distance,
							  float& min_t,
							  float& max_t,
							  Hit& rayHit) {
	Vec3f k;
	k.substract(rayPov, center);
	float c = k * k - sqrRadius;
	return raySphereIntersection(rayDir, k, c, distance, min_t, max_t, rayHit);
}

bool	raySphereIntersection(const Vec3f& rayDir,
							  const Vec3f& k,
							  float c,
							  float& distance,
							  float& min_t,
							  float& max_t,
							  Hit& rayHit) {
	float b = rayDir * k;
	float d = b * b - c;
	if (d >= 0) {
		float sqrt_d = std::sqrt(d);
		float t1 = -b + sqrt_d;
		float t2 = -b - sqrt_d;
		min_t = std::min(t1,t2);
		max_t = std::max(t1,t2);
		if (rayHit == FRONT) {
			if (min_t >= 0) {
				distance = min_t;
				if ( almostEqual(d, 0, EPSILON) ) {
					rayHit = OUTLINE;
				} else {
					rayHit = OUTSIDE;
				}
				return true;
			} else if (max_t > 0) {
				distance = max_t;
				rayHit = INSIDE;
				return true;
			}
		} else if (rayHit == BACK) {
			if (max_t > 0) {
				distance = max_t;
				rayHit = INSIDE;
				return true;
			}
		} else if (rayHit == OUTLINE && almostEqual(d, 0, EPSILON)) {
			distance = min_t >= 0 ? min_t : max_t;
			if (distance > 0) {
				rayHit = OUTSIDE;
				return true;
			}
		}
	}
	return false;
}

void	normalToRaySphereIntersect(const Vec3f& intersectPt,
								   const Vec3f& center,
								   Vec3f& normal) {
	normal.substract(intersectPt, center).normalize();
}

bool	rayPlaneIntersection(const Vec3f& rayDir,
							 const Vec3f& rayPov,
							 const Vec3f& center,
							 const Vec3f& normal,
							 float& distance,
							 float& min_t,
							 float& max_t,
							 Hit& rayHit) {
	distance = (normal * center - normal * rayPov) / (normal * rayDir);
	if (distance >= 0) {
		min_t = max_t = distance;
		rayHit = OUTSIDE;
		return true;
	}
	return false;
}

bool	rayRectangleIntersection(const Vec3f& rayDir,
								 const Vec3f& rayPov,
								 const Vec3f& center,
								 const Vec3f& normal,
								 float& distance,
								 float& min_t,
								 float& max_t,
								 Hit& rayHit) {
	if (rayPlaneIntersection(rayDir, rayPov, center, normal, distance, min_t, max_t, rayHit)) {
		
	}
	return false;
}
