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
	lookats = other.lookats;
	color = other.color;
}

bool Light::checkLookatsIdx(int idx) const {
	if (idx >= 0 && idx < (int)lookats.size()) {
		return true;
	}
	std::cerr << "Warning: lookats index is out of range" << std::endl;
	return false;
}

void Light::set_lookatCamera(const Position& eye, const LookatAux& aux) {
	set_lookatBase(eye);
	lookats.back().lookAt(eye, aux);
}

void Light::set_lookatBase(const Position& eye) {
	(void)eye;
	lookats.push_back(Lookat(_pos));
}

void Light::recalculateLookat(int cam, const Position& eye, const LookatAux& aux) {
	if (checkLookatsIdx(cam)) {
		bool pIsNull = lookats[cam].p.isNull();
		lookats[cam].lookAt(eye, aux);
		if (pIsNull) lookats[cam].p.toNull();
	}

}

void Light::recalculateLookat(int cam, float roll, const Vec3f& newPov) {
	if (checkLookatsIdx(cam)) {
		(void)newPov;
		bool pIsNull = lookats[cam].p.isNull();
		lookats[cam].set_roll(roll);
		if (pIsNull) lookats[cam].p.toNull();
	}
}

bool Light::intersection(Ray& ray, int cam, Hit rayHit) const {
	(void)ray;
	(void)cam;
	(void)rayHit;
	return false;
}

void Light::getNormal(Ray& ray, int cam) const {
	if (checkLookatsIdx(cam)) {
		ray.pov.addition(ray.pov, ray.dir * ray.dist);
		ray.norm.substract(lookats[cam].p, ray.pov).normalize();
		float k = ray.dir * ray.norm;
		if (k < 0) {
			k = -k;
		}
		ray.light = color;
		ray.light.product(k);
		ray.hits++;
	}
}

bool Light::lighting(Ray& ray, int cam) const {
	if (checkLookatsIdx(cam)) {
		if (lookats[cam].p.isInf() && !lookats[cam].n.isNull()) {// Sunlight
			float k = ray.norm * lookats[cam].n;
			if (k > 0) {
				ray.dist = INFINITY;
				ray.dir = lookats[cam].n;
				ray.light = light.light;
				ray.light.product(k);
				return true;
			}
		} else if (!lookats[cam].p.isInf() && lookats[cam].n.isNull()) {// Spot light
			ray.dist = ray.dir.substract(lookats[cam].p, ray.pov).norm();
			ray.dir.normalize();
			float k = ray.norm * ray.dir;
			if (k > 0) {
				ray.light = light.light;
				ray.light.product(k);
				return true;
			}
		} else if (!lookats[cam].p.isInf() && !lookats[cam].n.isNull()) {// Sunlight from the plane
			float k = ray.norm * lookats[cam].n;
			if (k > 0) {
				rayPlaneIntersection(ray.pov, ray.dir, lookats[cam].p, lookats[cam].n, ray.dist);
				ray.dir = lookats[cam].n;
				ray.light = light.light;
				ray.light.product(k);
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
	is >> l._pos.p >> l.light >> l._pos.n;
	l._pos.n.normalize();
	return is;
}

