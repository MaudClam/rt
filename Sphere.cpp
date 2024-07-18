//
//  Sphere.cpp
//  rt
//
//  Created by uru on 08/07/2024.
//

#include "Sphere.hpp"

Sphere::Sphere(void) : radius(0) {
	name = "sphere";
	nick = "sp";
}

Sphere::~Sphere(void) {}

Sphere::Sphere(const Vec3f& center, float radius, const ARGBColor& color) : radius(radius) {
	this->pos.p = center;
	this->color = color;
}

Sphere::Sphere(const Sphere& other) : radius(other.radius) {
	name = other.name;
	nick = other.nick;
	light = other.light;
	brightness = other.brightness;
	pos = other.pos;
	lookats = other.lookats;
	color = other.color;
}

bool Sphere::intersection(Ray& ray) const {//FIXME
	Vec3f k = ray.pov - pos.p;
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
}//FIXME

bool Sphere::intersection(Ray& ray, int cameraIdx) const {
	if (checkLookatsIdx(cameraIdx)) {
		Vec3f k; k.substract(ray.pov, lookats[cameraIdx].p);
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
	}
	return false;
}

std::ostream& operator<<(std::ostream& o, Sphere& sp) {
	o	<< sp.nick
		<< " " << sp.pos.p
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
		is >> sp.pos.p >> sp.radius;
		is >> sp.color;
		sp.radius /= 2;
	}
	return is;
}
