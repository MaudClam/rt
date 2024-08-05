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

Light::Light(const Light& other) : _light(other._light) {
	_pos = other._pos;
	color = other.color;
}

void Light::set_nick(const std::string& nick) { _nick = nick; }

void Light::set_name(const std::string& name) { _name = name; }

void Light::set_type(Type type) { _type = type; }


bool Light::intersection(Ray& ray, Hit rayHit) const {
	(void)ray;
	(void)rayHit;
	return false;
}

void Light::getNormal(Ray& ray) const {
	(void)ray;
}

bool Light::lighting(Ray& ray) const {
	if (_type == SPOTLIGHT) {
		ray.dist = ray.dir.substract(_pos.p, ray.pov).norm();
		ray.dir.normalize();
		float k = ray.norm * ray.dir;
		if (k > 0) {
			ray.light = _light.light;
			ray.light.product(k);
			return true;
		}
	} else if (_type == SUNLIGHT) {
		float k = ray.norm * _pos.n;
		if (k > 0) {
			ray.dist = INFINITY;
			ray.dir = _pos.n;
			ray.light = _light.light;
			ray.light.product(k);
			return true;
		}
	} else if (SUNLIGHT_LIMITED) {
		float k = ray.norm * _pos.n;
		if (k > 0) {
			rayPlaneIntersection(ray.pov, ray.dir, _pos.p, _pos.n, ray.dist);
			ray.dir = _pos.n;
			ray.light = _light.light;
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
	os << "   " << l._light;
	if (!l._pos.n.isNull()) {
		os << " " << l._pos.n;
	}
	o  << std::setw(46) << std::left << os.str();
	o  << " #" << l._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Light& l) {
	switch (l._type) {
		case Light::SPOTLIGHT : {
			is >> l._pos.p >> l._light;
			break;
		}
		case Light::SUNLIGHT : {
			is >> l._pos.n >> l._light;
			l._pos.n.normalize();
			break;
		}
		case Light::SUNLIGHT_LIMITED : {
			is >> l._pos.n >> l._light >> l._pos.p;
			l._pos.n.normalize();
			break;
		}
		default:
			break;
	}
	return is;
}

