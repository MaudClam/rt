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
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<> dis(0.0, 1.0);
	return dis(gen);
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

bool	rayHitDefinition(float& min_t, float& max_t, float& distance, Hit& rayHit) {
	if (rayHit == BACK) {
		if (max_t > 0) {
			distance = max_t;
			rayHit = INSIDE;
			return true;
		}
	} else {
		if (min_t >= 0) {
			distance = min_t;
			if (rayHit != OUTLINE)
				rayHit = OUTSIDE;
			return true;
		} else if (max_t > 0) {
			distance = max_t;
			rayHit = INSIDE;
			return true;
		}
	}
	return false;
}

float	getShining(const Vec3f& dirFromPov, const Vec3f& normal, const Vec3f& dirToLight, float glossy) {
	return getShining_(dirFromPov, normal, dirToLight, glossy);
}

float	getShining_(const Vec3f& dirFromPov, const Vec3f& normal, Vec3f dirToLight, float glossy) {
	if (glossy) {
		float k = dirFromPov * dirToLight.reflect(normal);
		if (k > 0)
			return std::pow(k, glossy);
	}
	return 0;
}

float	getSchlick(float cosine, float eta) {
	float r0 = (1 - eta) / (1 + eta);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}

long	elapsedTimeMs(const decltype(std::chrono::high_resolution_clock::now())& start, bool condition) {
	if (condition) {
		auto finish = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
	}
	return 0;
}

void displayTimeMs(long ms, std::string hint) {
	if (ms > 0) {
		long d = ms / 86400000; ms -= (d * 86400000);
		long h = ms / 3600000; ms -= (h * 3600000);
		long m = ms / 60000; ms -= (m * 60000);
		long s = ms / 1000; ms -= (s * 1000);
		if (!hint.empty())std::cout << hint << ": ";
		if (d) std::cout << d << "d:";
		if (d || h) std::cout << h << "h:";
		if (d || h || m) std::cout << m << "m:";
		if (d || h || m || s) std::cout << s << "s:";
		if (d || h || m || s || ms) std::cout << ms << "ms" << std::endl;
	}
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
ratio(1,1),
txtr(NULL)
{ u.x = 1; v.y = 1; pos.n.z = 1; }

A_Planar::A_Planar(Texture2* _txtr) :
pos(),
u(),
v(),
angle(0),
ratio(1,1),
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
		ratio = other.ratio;
		txtr = other.txtr;
	}
	return *this;
}


// struct Plane

Plane::Plane(Texture2* _txtr) : A_Planar(_txtr) {}

Plane::~Plane(void) {}


// struct Circle

Circle::Circle(void) : A_Planar(), r(0), sqR(0) {}

Circle::Circle(Texture2* _txtr) : A_Planar(_txtr), r(0), sqR(0) {}

Circle::Circle(const Circle& other) : A_Planar(), r(0), sqR(0) { *this = other; }

Circle::~Circle(void) {}

Circle& Circle::operator=(const Circle& other) {
	if (this != &other) {
		pos = other.pos;
		u = other.u;
		v = other.v;
		angle = other.angle;
		ratio = other.ratio;
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
		ratio = other.ratio;
		txtr = other.txtr;
		w_2 = other.w_2;
		h_2 = other.h_2;
	}
	return *this;
}


// struct Square

Square::Square(void) : Rectangle() {}

Square::Square(Texture2* _txtr) : Rectangle(_txtr) {}

Square::Square(const Square& other) : Rectangle(other) {}

Square::~Square(void) {}


// struct Sphere 2
Sphere2::Sphere2(void) :
pos(),
r(0),
sqR(0),
u(),
v(),
angle(0),
ratio(1,1),
txtr(NULL)
{ u.x = 1; v.y = 1; pos.n.z = 1; }

Sphere2::Sphere2(Texture2* _txtr) :
pos(),
r(0),
sqR(0),
u(),
v(),
angle(0),
ratio(8,4),
txtr(_txtr)
{ u.x = 1; v.y = 1; pos.n.z = 1; }

Sphere2::Sphere2(const Sphere2 &other) : Sphere2() { *this = other; }

Sphere2&	Sphere2::operator=(const Sphere2& other) {
	if (this != &other) {
		pos = other.pos;
		r = other.r;
		sqR = other.sqR;
		u = other.u;
		v = other.v;
		angle = other.angle;
		ratio = other.ratio;
		txtr = other.txtr;
	} return *this;
}

Sphere2::~Sphere2(void) {}

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
		if (std::abs(d) <= OUTLINE_WIDTH)
			rayHit = OUTLINE;
		return rayHitDefinition(min_t, max_t, distance, rayHit);
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
