#include "geometry.hpp"


// Non class functions

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

bool	almostEqual(float a, float b, float precision) { return std::fabs(a - b) < precision; }

bool	almostMore(float a, float b, float precision) { return a - b > precision; }

bool	almostLess(float a, float b, float precision) { return b - a > precision; }

float	degree2radian(float degree) {
	return (degree * M_PI_180);
}

float	radian2degree(float radian) {
	return (radian * M_180_PI);
}

float	loop_(float n, float lim) {
	lim = std::abs(lim);
	if (n >= 0 && n < lim)
		return n;
	int mult = (int)(n / lim);
	n = n - lim * mult;
	n = (n < 0 ? lim + n : n);
	return n;
}

int		loop_(int n, int lim) {
	lim = std::abs(lim);
	if (n >= 0 && n < lim)
		return n;
	n = n % lim;
	n = (n < 0 ? lim + n : n);
	return n;
}

float	reverse_(float n, float lim) {
	lim = std::abs(lim);
	if (n >= 0 && n <= lim)
		return n;
	n = std::abs(n);
	int mult = (int)(n / lim);
	n = n - lim * mult;
	n = (mult % 2 ? lim - n : n);
	return n;
}

int		reverse_(int n, int lim) {
	lim = std::abs(lim);
	if (n >= 0 && n <= lim)
		return n;
	n = std::abs(n);
	int mult = n / lim;
	n = n % lim;
	n = (mult % 2 ? lim - n : n);
	return n;
}

float	cosineDistr(float x) {
	int sign = x < 0 ? -1 : 1;
	return sign * (1.0 / M_PI) * std::sqrt(1.0 - x * x);
}

float	inverseCumulativeDistr(float u) {
	return std::cos(u * M_PI_2);
}

std::string  roundedString(float num, int factor) {
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

void	dabugPrint(int each, float param1, float param2, int factor) {
	static int i;

	i++;
	if (!(i % each)) {
		std::cout<< std::setw(factor + 6) << std::right << std::setfill(' ');
		std::cout << roundedString(param1,factor) << "\t" ;
		if (param2 != -1) {
			std::cout<< std::setw(factor + 6) << std::right << std::setfill(' ');
			std::cout << roundedString(param2, factor) << "\t" ;
		}
		std::cout<< std::setw(factor + 8) << std::right << std::setfill(' ');
		std::cout << i <<std::endl;
	}
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



// Soft Sadow

float	softShadow(float distToEdge,
				 float distToShader,
				 float lenght,
				 float softness) {
	if (distToEdge > 0.) {
		float d = std::pow( lenght * distToEdge / distToShader, softness );
		if (d < 0.003922)
			return 1.;
		if (d > 0.996078)
			return 0.;
		else
			return 1. - d;
	}
	return 0.;
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
				if ( almostEqual(distance, 0, EPSILON) ) {
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

float	distanceToSphericalShaderEdge(const Vec3f& intersectPt,
									  const Vec3f& center,
									  const Vec3f& dirToLight,
									  float radius) {
	(void)radius;
	float d = (center - intersectPt).normalize() * dirToLight * radius;
	return 	d > 0. ? d : 0.;
}

bool	rayPlaneIntersection(const Vec3f& pos,
							 const Vec3f& dir,
							 const Vec3f& point,
							 const Vec3f& norm,
							 float& distance) {
	float k = dir * norm;
	if ( !almostEqual(k, 0, EPSILON) ) {
		Vec3f r;
		r.substract(pos, point);
		float t = r.product(norm) / -k;
		if (t > 0) {
			distance = t;
			return true;
		}
	}
	return false;
}
