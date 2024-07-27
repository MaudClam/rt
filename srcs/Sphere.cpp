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
	_light = false;
}

Sphere::~Sphere(void) {}

Sphere::Sphere(const Vec3f& center, float radius, const ARGBColor& color) : _radius(radius) {
	_name = "sphere";
	_nick = "sp";
	_light = false;
	this->_pos.p = center;
	this->color = color;
}

Sphere::Sphere(const Sphere& other) : _radius(other._radius) {
	_name = other._name;
	_nick = other._nick;
	_light = other._light;
	_pos = other._pos;
	lookats = other.lookats;
	color = other.color;
}

bool Sphere::intersection(Ray& ray, int cam, Side side) const {
	if (checkLookatsIdx(cam)) {
		Vec3f	k;
		k.substract(ray.pov, lookats[cam].p);
		float b = ray.dir * k;
		float c = k * k - _radius * _radius;
		float d = b * b - c;
		if (d >= 0) {
			float sqrt_d = std::sqrt(d);
			float t1 = -b + sqrt_d;
			float t2 = -b - sqrt_d;
			float min_t = std::min(t1,t2);
			float max_t = std::max(t1,t2);
			if (side == FRONT) {
				float t = min_t >= 0 ? min_t : max_t;
				if (t > 0) {
					ray.dist = t;
					return true;
				}
			} else if (side == BACK) {
				if (max_t > 0) {
					ray.dist = max_t;
					return true;
				}
			}
		}
	}
	return false;
}

void Sphere::hit(Ray& ray, int cam) const {
	if (checkLookatsIdx(cam)) {
		ray.pov.addition(ray.pov, ray.dir * ray.dist);
		ray.norm.substract(lookats[cam].p, ray.pov).normalize();
		float k = ray.dir * ray.norm;
		if (k < 0) {
			k = -k;
		}
		ray.tmpColor = color;
		ray.tmpColor.product(k);
		ray.hits++;
	}
}

void Sphere::lighting(Ray& ray, int cam) const {
	(void)ray;
	(void)cam;
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
