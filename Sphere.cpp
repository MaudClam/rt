//
//  Sphere.cpp
//  rt
//
//  Created by uru on 08/07/2024.
//

#include "Sphere.hpp"

Sphere::Sphere(void) : radius(), lookats() {
	name = "sphere";
	nick = "sp";
}

Sphere::~Sphere(void) {}

Sphere::Sphere(const Vec3f& center, float radius, const ARGBColor& color) {
	this->center = center;
	this->radius = radius;
	this->color = color;
}

Sphere::Sphere(const Sphere& other) { *this = other; }

Sphere& Sphere::operator=(const Sphere& other) {
	if (this != &other) {
		center = other.center;
		color = other.color;
		radius = other.radius;
		lookats = other.lookats;
	}
	return *this;
}

bool Sphere::intersection(Ray& ray) const {
	Vec3f k = ray.pov - center;
	float b = ray.dir * k;
	float c = k * k - radius * radius;
	float d = b * b - c;
	if (d >= 0) {
		float sqrt_d = std::sqrt(d);
		float t1 = -b + sqrt_d;
		float t2 = -b - sqrt_d;
		float min_t = std::min(t1,t2);
		float max_t = std::max(t1,t2);
		float t = min_t >= 0 ? min_t : max_t;
		if (t > 0) {
			if (t < ray.dist) {
				ray.dist = t;
			}
			return true;
		}
	}
	return false;
}

bool Sphere::intersection(Ray& ray, int currentCamera) const {
	Vec3f k = ray.pov - lookats[currentCamera].pos;
	float b = ray.dir * k;
	float c = k * k - radius * radius;
	float d = b * b - c;
	if (d >= 0) {
		float sqrt_d = std::sqrt(d);
		float t1 = -b + sqrt_d;
		float t2 = -b - sqrt_d;
		float min_t = std::min(t1,t2);
		float max_t = std::max(t1,t2);
		float t = min_t >= 0 ? min_t : max_t;
		if (t > 0) {
			if (t < ray.dist) {
				ray.dist = t;
			}
			return true;
		}
	}
	return false;
}

void Sphere::makeLookatsPositions(const Position& camera) {
	(void)camera;
	lookats.push_back(Position(this->center, this->normal));//FIXME
}

std::ostream& operator<<(std::ostream& o, Sphere& sp) {
	o	<< sp.nick
		<< " " << sp.center
		<< " " << sp.radius * 2
		<< " " << sp.color.rrggbb()
		<< "\t#" << sp.name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Sphere& sp) {
	if (!is.str().compare(0, sp.nick.size(), sp.nick)) {
		char trash;
		for (size_t i = 0; i < sp.nick.size(); ++i) {
			is >> trash;
		}
		is >> sp.center >> sp.radius;
		is >> sp.color;
		sp.radius /= 2;
	}
	return is;
}
