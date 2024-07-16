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

Position& Position::lookat(const Position& eye) {//FIXME
	(void)eye;//FIXME
	return *this;
}


