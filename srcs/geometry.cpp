//
//  geometry.cpp
//  rt
//
//  Created by uru on 16/07/2024.
//

#include "geometry.hpp"


// Struct Position

Position::Position(void) : p(), n() {}

Position::~Position(void) {}

Position::Position(const Vec3f& point, const Vec3f& norm) : p(point), n(norm) { n.normalize(); }

Position::Position(const Position& other) : p(other.p), n(other.n) {}

Position& Position::operator=(const Position& other) {
	if (this != &other) {
		p = other.p;
		n = other.n;
	}
	return *this;
};


// Struct Lookat

Lookat::Lookat(void) : _roll(0) {}

Lookat::~Lookat(void) {}

Lookat::Lookat(const Vec3f& point, const Vec3f& norm, float roll) : _roll(roll) {
	p = point;
	n = norm;
}

Lookat::Lookat(const Position& pos, float roll) : _roll(roll) {
	p = pos.p;
	n = pos.n;
}

Lookat::Lookat(const Lookat& other) : _roll(other._roll) {
	p = other.p;
	n = other.n;
}

Lookat Lookat::operator=(const Lookat& other) {
	p = other.p;
	n = other.n;
	_roll = other._roll;
	return *this;
}

float	Lookat::get_roll(void) { return _roll; }

void	Lookat::set_roll(float roll) {
	p.turnAroundZ(roll - _roll);
	n.turnAroundZ(roll - _roll).normalize();
	_roll = roll;
}

Lookat& Lookat::lookAt(const Position& eye) {
	LookatAux aux(eye.n);
	lookAt(eye, aux);
	return *this;
}

Lookat& Lookat::lookAt(const Position& eye, const LookatAux& aux) {
	if (_roll != 0) {
		p.turnAroundZ(-_roll);
		n.turnAroundZ(-_roll).normalize();
	}
	n.lookatDir(aux);
	p.lookatPt(eye.p, aux);
	if (_roll != 0) {
		p.turnAroundZ(_roll);
		n.turnAroundZ(_roll).normalize();
	}
	return *this;
}


// Non member functions

bool almostEqual(float a, float b, int precision) { return std::fabs(a - b) < precision; }

bool almostEqual(double a, double b, int precision) { return std::fabs(a - b) < precision; }

float radian(float degree) {
	return (degree * std::numbers::pi / 180.);
}

float degree(float radian) {
	return (radian * 180. / std::numbers::pi);
}

// Intersections, normals, rays

Vec3f reflectRay(const Vec3f& norm, const Vec3f& dir) {
	Vec3f R(norm);
	R.product(dir * norm * 2);
	R.substract(R, dir).normalize();
	return R;
}

bool raySphereIntersection(const Vec3f& rayDir,
						   const Vec3f& pov_center,
						   float sqrRadius,
						   float& distance,
						   Hit rayHit) {
	float b = rayDir * pov_center;
	float c = pov_center * pov_center - sqrRadius;
	float d = b * b - c;
	if (d >= 0) {
		float sqrt_d = std::sqrt(d);
		float t1 = -b + sqrt_d;
		float t2 = -b - sqrt_d;
		float min_t = std::min(t1,t2);
		float max_t = std::max(t1,t2);
		if (rayHit == FRONT) {
			float t = min_t >= 0 ? min_t : max_t;
			if (t > 0) {
				distance = t;
				return true;
			}
		} else if (rayHit == BACK) {
			if (max_t > 0) {
				distance = max_t;
				return true;
			}
		} else if (rayHit == OUTLINE && almostEqual(d, 0, EPSILON)) {
			float t = min_t >= 0 ? min_t : max_t;
			if (t > 0) {
				distance = t;
				return true;
			}
		}
	}
	return false;
}

void normalToRaySphereIntersect(const Vec3f& intersectPt, const Vec3f& center, Vec3f& normal) {
	normal.substract(intersectPt, center).normalize();
}

bool rayPlaneIntersection(const Vec3f& rayPov,
						  const Vec3f& rayDir,
						  const Vec3f& point,
						  const Vec3f& norm,
						  float& distance) {
	float k = rayDir * norm;
	if ( !almostEqual(k, 0, EPSILON) ) {
		Vec3f r;
		r.substract(rayPov, point);
		float t = r.product(norm) / -k;
		if (t > 0) {
			distance = t;
			return true;
		}
	}
	return false;
}
