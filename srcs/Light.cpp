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

bool Light::intersection(Ray& ray, int cam, Side side) const {
	(void)ray;
	(void)cam;
	(void)side;
	return false;
}

void Light::hit(Ray& ray, int cam) const {
	if (checkLookatsIdx(cam)) {
		ray.pov.addition(ray.pov, ray.dir * ray.dist);
		ray.norm.substract(lookats[cam].p, ray.pov).normalize();
		float k = ray.dir * ray.norm;
		if (k < 0) {
			k = -k;
		}
		ray.tmpColor = color;
		ray.tmpColor.product(k);
		ray.hits++;
	}
}

void Light::lighting(Ray& ray, int cam) const {
	(void)ray;
	(void)cam;
}

void Light::output(std::ostringstream& os) {
	os << *this;
}


// Non member functions

std::ostream& operator<<(std::ostream& o, Light& l) {
	std::ostringstream os;
	os << std::setw(2) << std::left << l._nick;
	os << " " << l._pos.p;
	os << "   " << l.light;
	if (!l._pos.n.isNull()) {
		os << " " << l._pos.n;
	}
	o  << std::setw(36) << std::left << os.str();
	o  << " #" << l._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Light& l) {
	is >> l._pos.p >> l.light >> l._pos.n;
	l._pos.n.normalize();
	return is;
}

