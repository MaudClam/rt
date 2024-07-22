//
//  Sphere.cpp
//  rt
//
//  Created by uru on 08/07/2024.
//

#include "Sphere.hpp"

Sphere::Sphere(void) : _radius(0) {
	_name = "sphere";
	_nick = "sp";
	_light = false;
}

Sphere::~Sphere(void) {}

Sphere::Sphere(const Vec3f& center, float radius, const ARGBColor& color) : _radius(radius) {
	_name = "sphere";
	_nick = "sp";
	_light = false;
	this->_pos.p = center;
	this->color = color;
}

Sphere::Sphere(const Sphere& other) : _radius(other._radius) {
	_name = other._name;
	_nick = other._nick;
	_light = other._light;
	_brightness = other._brightness;
	_pos = other._pos;
	lookats = other.lookats;
	color = other.color;
}

bool Sphere::intersection(Ray& ray, int cameraIdx, float roll, Side side) const {
	if (checkLookatsIdx(cameraIdx)) {
		Vec3f		k;
		Position	r(lookats[cameraIdx]);
		r.rollingRadian(roll);
		k.substract(ray.pov, r.p);
		float b = ray.dir * k;
		float c = k * k - _radius * _radius;
		float d = b * b - c;
		if (d >= 0) {
			float sqrt_d = std::sqrt(d);
			float t1 = -b + sqrt_d;
			float t2 = -b - sqrt_d;
			float min_t = std::min(t1,t2);
			float max_t = std::max(t1,t2);
			if (side == FRONT) {
				float t = min_t >= 0 ? min_t : max_t;
				if (t > 0) {
					ray.dist = t;
					return true;
				}
			} else if (side == BACK) {
				if (max_t > 0) {
					ray.dist = max_t;
					return true;
				}
			}
		}
	}
	return false;
}

std::ostream& operator<<(std::ostream& o, Sphere& sp) {
	o	<< sp._nick
		<< " " << sp._pos.p
		<< " " << sp._radius * 2
		<< " " << sp.color.rrggbb()
		<< "\t#" << sp._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Sphere& sp) {
	if (!is.str().compare(0,sp._nick.size(),sp._nick)) {
		char trash;
		for (size_t i = 0; i < sp._nick.size(); ++i) {
			is >> trash;
		}
		is >> sp._pos.p >> sp._radius;
		is >> sp.color;
		sp._radius /= 2;
	}
	return is;
}
