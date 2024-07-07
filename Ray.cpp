//
//  Ray.cpp
//  rt
//
//  Created by uru on 05/07/2024.
//

#include "Ray.hpp"

Ray::Ray(void) : pov(), normal(), color(), generation(0) {}

Ray::~Ray(void) {}

Ray::Ray(const Vec3f& p, const Vec3f& n, const ARGBColor& c, int g) :
pov(p), normal(n), color(c), generation(g) {}

Ray::Ray(const Ray& other) :
pov(other.pov), normal(other.normal), color(other.color), generation(other.generation) {}

Ray& Ray::operator=(const Ray& other) {
	if (this != &other) {
		pov = other.pov;
		normal = other.normal;
		color = other.color;
		generation = other.generation;
	}
	return *this;
}
