//
//  Lighting.cpp
//  rt
//
//  Created by uru on 25/07/2024.
//

#include "Light.hpp"

Light::Light(void) {
	_name = "light";
	_nick = "l";
	_light = true;
}

Light::~Light(void) {}

Light::Light(const Light& other) : light(other.light) {
	_pos = other._pos;
	lookats = other.lookats;
	color = other.color;
}

bool Light::intersection(Ray& ray, int cam, float roll, Side side) const {
	(void)ray;
	(void)cam;
	(void)roll;
	(void)side;
	return false;
}

void Light::lighting(Ray& ray, int cam, float roll) const {
	(void)ray;
	(void)cam;
	(void)roll;
}

void Light::hit(Ray& ray, int cam, float roll) const {
	if (checkLookatsIdx(cam)) {
		Position center(lookats[cam]);
		center.rolling(roll);
		ray.pov.addition(ray.pov, ray.dir * ray.dist);
		ray.norm.substract(center.p, ray.pov).normalize();
		ray.norm.turnAroundZ(roll);
		float k = ray.dir * ray.norm;
		if (k < 0) {
			k = -k;
		}
		ray.tmpColor = color;
		ray.tmpColor.product(k);
		ray.hits++;
	}
}

std::ostream& operator<<(std::ostream& o, Light& l) {
	o	<< l._nick
		<< " " << l._pos.p
		<< " " << l._pos.p
		<< " " << l.light
		<< "\t#" << l._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Light& l) {
	if (!is.str().compare(0,l._nick.size(),l._nick)) {
		char trash;
		for (size_t i = 0; i < l._nick.size(); ++i) {
			is >> trash;
		}
		is >> l._pos.p >> l.light;
	}
	return is;
}

