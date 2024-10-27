//
//  Sphere.cpp
//  rt
//
//  Created by uru on 08/07/2024.
//

#include "Sphere.hpp"

Sphere::Sphere(void) : _radius(0), _sqrRadius(0), _k(), _c(0) {
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

Sphere::Sphere(const Sphere& other) :
_radius(other._radius),
_sqrRadius(_radius * _radius),
_k(other._k),
_c(other._c)
{
	_name = other._name;
	_nick = other._nick;
	_isLight = other._isLight;
	_pos = other._pos;
	color = other.color;
	combineType = other.combineType;
	specular = other.specular;
	reflective = other.reflective;
	refractive = other.refractive;
	matIOR = other.matIOR;
	matOIR = other.matOIR;
}

Sphere* Sphere::clone(void) const {
	Sphere* sphere = new Sphere(*this);
	return sphere;
}

void Sphere::lookat(const Position& eye, const LookatAux& aux, const Vec3f& pos, float roll) {
	_pos.lookat(eye, aux, roll);
	_k.substract(pos,_pos.p);
	_c = _k * _k - _sqrRadius;
}

void Sphere::roll(const Vec3f& pos, float shiftRoll) {
	if (shiftRoll != 0) {
		_pos.roll(shiftRoll);
		_k.substract(pos,_pos.p);
		_c = _k * _k - _sqrRadius;
	}
}

bool Sphere::intersection(Ray& ray) const {
	bool result = false;
	if (ray.hit == ANY_SHADOW || ray.hit == FIRST_SHADOW) {
		ray.hit = FRONT;
		result = raySphereIntersection(ray.dirL, ray.pov, _pos.p, _sqrRadius,
									   ray.dist, ray.intersections.a.d, ray.intersections.b.d,
									   ray.hit);
	} else if (!ray.recursion) {
		result = raySphereIntersection(ray.dir, _k, _c, ray.dist,
									   ray.intersections.a.d, ray.intersections.b.d,
									   ray.hit);
	} else {
		result = raySphereIntersection(ray.dir, ray.pov, _pos.p, _sqrRadius,
									   ray.dist, ray.intersections.a.d, ray.intersections.b.d,
									   ray.hit);
	}
	return result;
}

void Sphere::giveNormal(Ray& ray) const {
	ray.movePovByDirToDist();
	if (ray.hit == INSIDE) {
		normalToRaySphereIntersect(_pos.p, ray.pov, ray.norm);
	} else {
		normalToRaySphereIntersect(ray.pov, _pos.p, ray.norm);
	}
}

float Sphere::getDistanceToShaderEdge(Ray& ray, float distance, bool inside) const {
	if (inside) {
		return distanceToSphericalShaderEdge(_pos.p,
											 ray.pov + (ray.dirL * distance),
											 ray.dirL,
											 _radius);
	}
	return distanceToSphericalShaderEdge(ray.pov + (ray.dirL * distance),
										 _pos.p,
										 ray.dirL,
										 _radius);
}

float Sphere::lighting(Ray& ray) const {
	(void)ray;
	return 0;
}

void Sphere::output(std::ostringstream& os) const {
	os << *this;
}

// Non member functions

std::ostream& operator<<(std::ostream& o, const Sphere& sp) {
	std::ostringstream os;
	os << std::setw(2) << std::left << sp._nick;
	os << " " << sp._pos.p;
	os << " " << std::setw(5) << std::right << sp._radius * 2;
	os << "   " << sp.color.rrggbb();
	os << " " << std::setw(4) << std::right << sp.specular;
	os << " " << std::setw(4) << std::right << sp.reflective;
	os << " " << std::setw(4) << std::right << sp.refractive;
	os << " " << std::setw(4) << std::right << sp.matIOR;
	o  << std::setw(54) << std::left << os.str();
	o  << " #" << sp._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Sphere& sp) {
	is >> sp._pos.p >> sp._radius;
	is >> sp.color >> sp.specular >> sp.reflective >> sp.refractive >> sp.matIOR;
	sp._radius /= 2;
	sp._sqrRadius = sp._radius * sp._radius;
	sp.specular = i2limits(sp.specular, -1, 1000);
	sp.reflective = f2limits(sp.reflective, 0, 1);
	sp.refractive = f2limits(sp.refractive, 0, 1);
	sp.matOIR = 1. / sp.matIOR;
	return is;
}
