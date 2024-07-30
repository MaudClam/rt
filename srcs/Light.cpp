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

void Light::set_lookatCamera(const Position& eye, const LookatAux& aux) {
	set_lookatBase();
	bool pIsNull = lookats.back().p.isNull();
	bool nIsNull = lookats.back().n.isNull();
	lookats.back().lookAt(eye, aux);
	if (pIsNull) lookats.back().p.toNull();
	if (nIsNull) lookats.back().n.toNull();
}

void Light::set_lookatBase(void) {
	lookats.push_back(Lookat(_pos));
}

void Light::recalculateLookat(int idx, const Position& eye, const LookatAux& aux) {
	if (checkLookatsIdx(idx)) {
		bool pIsNull = lookats[idx].p.isNull();
		lookats[idx].lookAt(eye, aux);
		if (pIsNull) lookats[idx].p.toNull();
	}

}

void Light::recalculateLookat(int idx, float roll) {
	if (checkLookatsIdx(idx)) {
		bool pIsNull = lookats[idx].p.isNull();
		lookats[idx].set_roll(roll);
		if (pIsNull) lookats[idx].p.toNull();
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
		if (lookats[cam].p.isNull() && !lookats[cam].n.isNull()) {// Sunlight
			float k = ray.norm * lookats[cam].n;
			if (k > 0) {
				ray.dist = INFINITY;
				ray.dir = lookats[cam].n;
				ray.light = light.light;
				ray.light.product(k);
				return true;
			}
		} else if (!lookats[cam].p.isNull() && lookats[cam].n.isNull()) {// Spot light
			ray.dist = ray.dir.substract(lookats[cam].p, ray.pov).norm();
			ray.dir.normalize();
			float k = ray.norm * ray.dir;
			if (k > 0) {
				ray.light = light.light;
				ray.light.product(k);
				return true;
			}
		} else if (!lookats[cam].p.isNull() && !lookats[cam].n.isNull()) {// Sunlight from the plane
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
	o  << std::setw(36) << std::left << os.str();
	o  << " #" << l._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Light& l) {
	is >> l._pos.p >> l.light >> l._pos.n;
	l._pos.n.normalize();
	return is;
}

