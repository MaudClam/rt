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
	_isLight = true;
}

Light::~Light(void) {}

Light::Light(const Light& other) : light(other.light) {
	_pos = other._pos;
	color = other.color;
}

bool Light::intersection(Ray& ray, Hit rayHit) const {
	(void)ray;
	(void)rayHit;
	return false;
}

void Light::getNormal(Ray& ray) const {
	(void)ray;
}

bool Light::lighting(Ray& ray) const {
	if (_pos.p.isInf() && !_pos.n.isNull()) {// Sunlight
		float k = ray.norm * _pos.n;
		if (k > 0) {
			ray.dist = INFINITY;
			ray.dir = _pos.n;
			ray.light = light.light;
			ray.light.product(k);
			return true;
		}
	} else if (!_pos.p.isInf() && _pos.n.isNull()) {// Spot light
		ray.dist = ray.dir.substract(_pos.p, ray.pov).norm();
		ray.dir.normalize();
		float k = ray.norm * ray.dir;
		if (k > 0) {
			ray.light = light.light;
			ray.light.product(k);
			return true;
		}
	} else if (!_pos.p.isInf() && !_pos.n.isNull()) {// Sunlight from the plane
		float k = ray.norm * _pos.n;
		if (k > 0) {
			rayPlaneIntersection(ray.pov, ray.dir, _pos.p, _pos.n, ray.dist);
			ray.dir = _pos.n;
			ray.light = light.light;
			ray.light.product(k);
			return true;
		}
	}
	return false;
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
	o  << std::setw(46) << std::left << os.str();
	o  << " #" << l._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Light& l) {
	is >> l._pos.p >> l.light >> l._pos.n;
	l._pos.n.normalize();
	return is;
}

