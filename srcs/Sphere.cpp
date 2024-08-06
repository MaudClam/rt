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
	color = other.color;
	specular = other.specular;
	reflective = other.reflective;
}

Sphere* Sphere::clone(void) const {
	Sphere* sphere = new Sphere(*this);
	return sphere;
}

void Sphere::lookat(const Position& eye, const LookatAux& aux, const Vec3f& pov) {
	_pos.lookat(eye, aux);
	_k.substract(pov,_pos.p);
	_c = _k * _k - _sqrRadius;
}

bool Sphere::intersection(Ray& ray, bool notOptimize, Hit rayHit) const {
	if (notOptimize) {
		return raySphereIntersection(ray.dir, ray.pov, _pos.p, _sqrRadius, ray.dist, rayHit);
	}
	return _raySphereIntersection(ray.dir, _k, _c, ray.dist, rayHit);
}

void Sphere::getNormal(Ray& ray) const {
	normalToRaySphereIntersect(ray.pov, _pos.p, ray.norm);
}

bool Sphere::lighting(Ray& ray) const {
	(void)ray;
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
