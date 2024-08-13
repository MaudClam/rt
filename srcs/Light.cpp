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

Light::Light(const Light& other) : _type(other._type), light(other.light) {
	_name = other._name;
	_nick = other._nick;
	_isLight = other._isLight;
	_pos = other._pos;
	color = other.color;
	specular = other.specular;
	reflective = other.reflective;
}

Light* Light::clone(void) const {
	Light* light = new Light(*this);
	return light;
}

void Light::set_nick(const std::string& nick) { _nick = nick; }

void Light::set_name(const std::string& name) { _name = name; }

void Light::set_type(Type type) { _type = type; }

void Light::lookat(const Position& eye, const LookatAux& aux, const Vec3f& pov, float roll) {
	(void)pov;
	_pos.lookat(eye, aux, roll);
}

void Light::roll(const Vec3f& pov, float roll) {
	(void)pov;
	_pos.roll(roll);
}

bool Light::intersection(Ray& ray) const {
	(void)ray;
	return false;
}

void Light::calculateNormal(Ray& ray) const {
	(void)ray;
}

bool Light::lighting(Ray& ray, const A_Scenery& scenery, const a_scenerys_t& scenerys) const {
	float k = 0;
	switch (_type) {
		case SPOTLIGHT: {
			ray.dist = ray.dirToLight.substract(_pos.p, ray.pov).norm();
			if (ray.dist != 0) (ray.dirToLight.product(1 / ray.dist));// optimal normalization
			if ( (k = ray.dirToLight * ray.norm) <= 0) {
				return false;
			}
			break;
		}
		case SUNLIGHT: {
			if ( (k = _pos.n * ray.norm) <= 0) {
				return false;
			}
			ray.dist = INFINITY;
			ray.dirToLight = _pos.n;
			break;
		}
		case SUNLIGHT_LIMITED: {
			if ( (k = _pos.n * ray.norm) <= 0) {
				return false;
			}
			rayPlaneIntersection(ray.pov, _pos.n, _pos.p, ray.norm, ray.dist);
			ray.dirToLight = _pos.n;
			break;
		}
		default:
			return false;
	}
	ray.movePovByEpsilon();
	if ( shadow(ray,scenerys) ) {
		return false;
	}
	ray.collectLight(scenery.color, light.light, k);
	ray.collectShine(scenery.color, light.light, scenery.specular);
	return true;
}

bool Light::shadow(Ray& ray, const a_scenerys_t& scenerys) const {
	float distance = ray.dist;
	for (auto scenery = scenerys.begin(), end = scenerys.end(); scenery != end; ++scenery) {
		ray.hit = FRONT_SHADOW;
		if ( (*scenery)->intersection(ray) ) {
			if (distance > ray.dist) {
				return true;
			}
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
	switch (l._type) {
		case Light::SPOTLIGHT : {
			is >> l._pos.p >> l.light;
			break;
		}
		case Light::SUNLIGHT : {
			is >> l._pos.n >> l.light;
			l._pos.n.normalize();
			break;
		}
		case Light::SUNLIGHT_LIMITED : {
			is >> l._pos.n >> l.light >> l._pos.p;
			l._pos.n.normalize();
			break;
		}
		default:
			break;
	}
	return is;
}

