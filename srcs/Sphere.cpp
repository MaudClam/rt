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
	_isLight = false;
}

Sphere::~Sphere(void) {}

Sphere::Sphere(const Vec3f& center, float radius, const ARGBColor& color) : _radius(radius) {
	_name = "sphere";
	_nick = "sp";
	_isLight = false;
	this->_pos.p = center;
	this->color = color;
}

Sphere::Sphere(const Sphere& other) : _radius(other._radius) {
	_name = other._name;
	_nick = other._nick;
	_isLight = other._isLight;
	_pos = other._pos;
	lookats = other.lookats;
	color = other.color;
}

bool Sphere::checkLookatsIdx(int idx) const {
	if (idx >= 0 && idx < (int)lookats.size()) {
		return true;
	}
	std::cerr << "Warning: lookats index is out of range" << std::endl;
	return false;
}

void Sphere::set_lookatCamera(const Position& eye, const LookatAux& aux) {
	set_lookatBase();
	lookats.back().lookAt(eye, aux);
}

void Sphere::set_lookatBase(void) {
	lookats.push_back(Lookat(_pos));
}

void Sphere::recalculateLookat(int idx, const Position& eye, const LookatAux& aux) {
	if (checkLookatsIdx(idx)) {
		lookats[idx].lookAt(eye, aux);
	}
}

void Sphere::recalculateLookat(int idx, float roll) {
	if (checkLookatsIdx(idx)) {
		lookats[idx].set_roll(roll);
	}
}

bool Sphere::intersection(Ray& ray, int cam, Hit rayHit) const {
	if (checkLookatsIdx(cam)) {
		return raySphereIntersection(ray.pov, ray.dir, lookats[cam].p, _radius, ray.dist, rayHit);
	}
	return false;
}

void Sphere::getNormal(Ray& ray, int cam) const {
	if (checkLookatsIdx(cam)) {
		normalToRaySphereIntersect(ray.pov, lookats[cam].p, ray.norm);
	}
}

bool Sphere::lighting(Ray& ray, int cam) const {
	(void)ray;
	(void)cam;
	return false;
}

void Sphere::output(std::ostringstream& os) {
	os << *this;
}

// Non member functions

std::ostream& operator<<(std::ostream& o, Sphere& sp) {
	std::ostringstream os;
	os << std::setw(2) << std::left << sp._nick;
	os << " " << sp._pos.p;
	os << " " << std::setw(4) << std::right << sp._radius * 2;
	os << "   " << sp.color.rrggbb();
	o  << std::setw(36) << std::left << os.str();
	o  << " #" << sp._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Sphere& sp) {
	is >> sp._pos.p >> sp._radius;
	is >> sp.color;
	sp._radius /= 2;
	return is;
}
