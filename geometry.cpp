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

Position& Position::lookat(const Position& eye) {
	LookatAux aux(eye.n);
	lookat(eye, aux);
	return *this;
}

Position& Position::lookat(const Position& eye, const LookatAux& aux) {
	if ( !(almostEqual(n.x, 0) && almostEqual(n.y, 0) && almostEqual(n.z, 0)) ) {
		n.lookatDir(aux);
	}
	p.lookatPt(eye.p, aux);
	return *this;
}

Position& Position::rolling(float roll) {
	if (roll != 0) {
		float cos = std::cos(roll), sin = std::sin(roll);
		float px = p.x * cos - p.y * sin, py = p.x * sin + p.y * cos;
		p.x = px; p.y = py;
		if ( !(n.x == 0 && n.y == 0) ) {
			float nx = n.x * cos - n.y * sin, ny = n.x * sin + n.y * cos;
			n.x = nx; n.y = ny;
			n.normalize();
		}
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
