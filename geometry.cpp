//
//  geometry.cpp
//  rt
//
//  Created by uru on 16/07/2024.
//

#include "geometry.hpp"

Position::Position(void) : p(), n() {}

Position::~Position(void) {}

Position::Position(const Vec3f& point, const Vec3f& norm) : p(point), n(norm) { n.normalize(); }

Position::Position(const Position& other) : p(other.p), n(other.n) {}

Position& Position::operator=(const Position& other) {
	if (this != &other) {
		p = other.p;
		n = other.n;
	}
	return *this;
};

Position& Position::rolling(float roll) {
	if (roll != 0) {
		float x = 0, y = 0, cos = std::cos(roll), sin = std::sin(roll);
		x = p.x * cos - p.y * sin;
		y = p.x * sin + p.y * cos;
		p.x = x; p.y = y;
		x = n.x * cos - n.y * sin;
		y = n.x * sin + n.y * cos;
		n.x = x; n.y = y;
	}
	return *this;
}

Position& Position::lookat(const Position& eye, const float roll) {
	float x, y, z;
	Vec3f dir(eye.n);
	Vec3f up;
	Vec3f right(0,-1,0);
	if (eye.n.x == 0 && (eye.n.y == -1 || eye.n.y == 1) && eye.n.z == 0) {
		right.y = 0; right.z = -1;
	}
	right.product(dir, right).normalize();
	up.product(dir, right).normalize();
	x = n * right; y = n * up; z = n * dir;
	n.x = x; n.y = y; n.z = z; n.normalize();
	p.substract(p, eye.p);
	x = p * right; y = p * up; z = p * dir;
	p.x = x; p.y = y; p.z = z; n.normalize();
	this->rolling(roll);
	return *this;
}

Position& Position::lookatBase(const Position& eye, float roll) {
	Position O(Vec3f(0,0,0),Vec3f(0,0,1));
	O.lookat(*this, 0);
	*this = eye;
	this->lookat(O, -roll);
	return *this;
}

float radian(float degree) {
	return (degree * std::numbers::pi / 180);
}

float degree(float radian) {
	return (radian * 180 / std::numbers::pi);
}
