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
	combineType = other.combineType;
	color = other.color;
	specular = other.specular;
	reflective = other.reflective;
	matIOR = other.matIOR;
	matOIR = other.matOIR;
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

void Light::giveNormal(Ray& ray) const {
	(void)ray;
}

float Light::getDistanceToShaderEdge(Ray& ray, float distance, bool inside) const {
	(void)ray;
	(void)distance;
	(void)inside;
	return 1.;
}

float Light::lighting(Ray& ray) const {
	float k = 0;
	switch (_type) {
		case SPOTLIGHT: {
			ray.dist = ray.dirL.substract(_pos.p, ray.pov).norm();
			if (ray.dist != 0) (ray.dirL.product(1 / ray.dist));// optimal normalization
			if ( (k = ray.dirL * ray.norm) <= 0) {
				return 0;
			}
			break;
		}
		case SUNLIGHT: {
			if ( (k = _pos.n * ray.norm) <= 0) {
				return 0;
			}
			ray.dist = _INFINITY;
			ray.dirL = _pos.n;
			break;
		}
		case SUNLIGHT_LIMITED: {
			if ( (k = _pos.n * ray.norm) <= 0) {
				return 0;
			}
			rayPlaneIntersection(ray.pov, _pos.n, _pos.p, ray.norm, ray.dist);
			ray.dirL = _pos.n;
			break;
		}
		default:
			return 0;
	}
	ray.light = light.light;
	return k;
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
	o  << std::setw(56) << std::left << os.str();
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

