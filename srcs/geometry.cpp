//
//  geometry.cpp
//  rt
//
//  Created by uru on 16/07/2024.
//

#include "geometry.hpp"


// Struct Position

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


// Struct Lookat

Lookat::Lookat(void) : _roll(0) {}

Lookat::~Lookat(void) {}

Lookat::Lookat(const Vec3f& point, const Vec3f& norm, float roll) : _roll(roll) {
	p = point;
	n = norm;
}

Lookat::Lookat(const Position& pos, float roll) : _roll(roll) {
	p = pos.p;
	n = pos.n;
}

Lookat::Lookat(const Lookat& other) : _roll(other._roll) {
	p = other.p;
	n = other.n;
}

Lookat Lookat::operator=(const Lookat& other) {
	p = other.p;
	n = other.n;
	_roll = other._roll;
	return *this;
}

float	Lookat::get_roll(void) { return _roll; }

void	Lookat::set_roll(float roll) {
	p.turnAroundZ(roll - _roll);
	n.turnAroundZ(roll - _roll).normalize();
	_roll = roll;
}

Lookat& Lookat::lookAt(const Position& eye) {
	LookatAux aux(eye.n);
	lookAt(eye, aux);
	return *this;
}

Lookat& Lookat::lookAt(const Position& eye, const LookatAux& aux) {
	if (_roll != 0) {
		p.turnAroundZ(-_roll);
		n.turnAroundZ(-_roll).normalize();
	}
	n.lookatDir(aux);
	p.lookatPt(eye.p, aux);
	if (_roll != 0) {
		p.turnAroundZ(_roll);
		n.turnAroundZ(_roll).normalize();
	}
	return *this;
}


// Non member functions

bool almostEqual(float a, float b, int precision) { return std::fabs(a - b) < precision; }

bool almostEqual(double a, double b, int precision) { return std::fabs(a - b) < precision; }

float radian(float degree) {
	return (degree * std::numbers::pi / 180.);
}

float degree(float radian) {
	return (radian * 180. / std::numbers::pi);
}
