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

bool raySphereIntersection(const Vec3f& rayDir,
						   const Vec3f& rayPov,
						   const Vec3f& center,
						   float sqrRadius,
						   float& distance,
						   Hit& rayHit) {
	Vec3f k;
	k.substract(rayPov, center);
	float c = k * k - sqrRadius;
	return raySphereIntersection(rayDir, k, c, distance, rayHit);
}

bool raySphereIntersection(const Vec3f& rayDir,
							const Vec3f& k,
							float c,
							float& distance,
							Hit& rayHit) {
	float b = rayDir * k;
	float d = b * b - c;
	if (d >= 0) {
		float sqrt_d = std::sqrt(d);
		float t1 = -b + sqrt_d;
		float t2 = -b - sqrt_d;
		float min_t = std::min(t1,t2);
		float max_t = std::max(t1,t2);
		if (rayHit == FRONT) {
			if (min_t >= 0) {
				distance = min_t;
				if (distance > 0) {
					rayHit = OUTSIDE;
					return true;
				}
			} else {
				distance = max_t;
				if (distance > 0) {
					rayHit = INSIDE;
					return true;
				}
			}
		} else if (rayHit == BACK) {
			if (max_t > 0) {
				distance = max_t;
				if (distance > 0) {
					rayHit = INSIDE;
					return true;
				}
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

void normalToRaySphereIntersect(const Vec3f& intersectPt, const Vec3f& center, Vec3f& normal) {
	normal.substract(intersectPt, center).normalize();
}

bool rayPlaneIntersection(const Vec3f& pov,
						  const Vec3f& dir,
						  const Vec3f& point,
						  const Vec3f& norm,
						  float& distance) {
	float k = dir * norm;
	if ( !almostEqual(k, 0, EPSILON) ) {
		Vec3f r;
		r.substract(pov, point);
		float t = r.product(norm) / -k;
		if (t > 0) {
			distance = t;
			return true;
		}
	}
	return false;
}
