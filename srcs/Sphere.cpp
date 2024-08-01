//
//  Sphere.cpp
//  rt
//
//  Created by uru on 08/07/2024.
//

#include "Sphere.hpp"

Sphere::Sphere(void) : _radius(0), _sqrRadius(0) {
	_name = "sphere";
	_nick = "sp";
	_isLight = false;
}

Sphere::~Sphere(void) {}

Sphere::Sphere(const Vec3f& center, float radius, const ARGBColor& color) :
_radius(radius), _sqrRadius(radius * radius) {
	_name = "sphere";
	_nick = "sp";
	_isLight = false;
	_pos.p = center;
	this->color = color;
}

Sphere::Sphere(const Sphere& other) : _radius(other._radius), _sqrRadius(_radius * _radius){
	_name = other._name;
	_nick = other._nick;
	_isLight = other._isLight;
	_pos = other._pos;
	lookats = other.lookats;
	color = other.color;
	specular = other.specular;
	reflective = other.reflective;
}

bool Sphere::checkLookatsIdx(int idx) const {
	if (idx >= 0 && idx < (int)lookats.size()) {
		return true;
	}
	std::cerr << "Warning: lookats index is out of range" << std::endl;
	return false;
}

void Sphere::set_lookatCamera(const Position& eye, const LookatAux& aux) {
	set_lookatBase(eye);
	size_t idx = lookats.size() - 2;
	lookats[idx].lookAt(eye, aux);
	lookats[idx + 1].p.substract(eye.p, lookats[idx].p);
}

void Sphere::set_lookatBase(const Position& eye) {
	lookats.push_back(Lookat(_pos));
	lookats.push_back( Lookat(eye.p - _pos.p, Vec3f()) );
}

void Sphere::recalculateLookat(int cam, const Position& eye, const LookatAux& aux) {
	int idx = cam * 2;
	if (checkLookatsIdx(idx + 1)) {
		lookats[idx].lookAt(eye, aux);
		lookats[idx + 1].p.substract(eye.p, lookats[idx].p);
	}
}

void Sphere::recalculateLookat(int cam, float roll, const Vec3f& newPov) {
	int idx = cam * 2;
	if (checkLookatsIdx(idx + 1)) {
		lookats[idx].set_roll(roll);
		lookats[idx + 1].p.substract(newPov, lookats[idx].p);
	}
}

bool Sphere::intersection(Ray& ray, int cam, Hit rayHit) const {
	int  idx = cam * 2; // pov_center idx
	if (checkLookatsIdx(idx)) {
		return raySphereIntersection(ray.dir, ray.pov - lookats[idx].p, _sqrRadius, ray.dist, rayHit);
	}
	return false;
}

void Sphere::getNormal(Ray& ray, int cam) const {
	int  idx = cam * 2; // center idx
	if (checkLookatsIdx(idx)) {
		normalToRaySphereIntersect(ray.pov, lookats[idx].p, ray.norm);
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
	os << " " << std::setw(5) << std::right << sp._radius * 2;
	os << "   " << sp.color.rrggbb();
	os << " " << std::setw(4) << std::right << sp.specular;
	os << " " << std::setw(4) << std::right << sp.reflective;
	o  << std::setw(46) << std::left << os.str();
	o  << " #" << sp._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Sphere& sp) {
	is >> sp._pos.p >> sp._radius;
	is >> sp.color >> sp.specular >> sp.reflective;
	sp._radius /= 2;
	sp._sqrRadius = sp._radius * sp._radius;
	sp.specular = i2limits(sp.specular, -1, 1000);
	sp.reflective = f2limits(sp.reflective, 0, 1);
	return is;
}
