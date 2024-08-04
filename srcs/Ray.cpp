//
//  Ray.cpp
//  rt
//
//  Created by uru on 01/08/2024.
//

#include "Ray.hpp"

// struct Ray

Ray::Ray(void) : pov(), dir(), dist(0), color(0) {}

Ray::~Ray(void) {}

Ray::Ray(const Ray& other) :
pov(other.pov),
dir(other.dir),
norm(other.norm),
dist(other.dist),
light(other.light),
color(other.color)
{}

Ray& Ray::operator=(const Ray& other) {
	if (this != & other) {
		pov = other.pov;
		dir = other.dir;
		norm = other.norm;
		dist = other.dist;
		light = other.light;
		color = other.color;
	}
	return *this;
}

void Ray::reflect(void) {
	dir.product(-1);
	dir.substract(norm * (2 * (norm * dir)), dir);
	pov.addition(pov, norm * EPSILON);
}

