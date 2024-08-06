//
//  Ray.cpp
//  rt
//
//  Created by uru on 01/08/2024.
//

#include "Ray.hpp"

// struct Ray

Ray::Ray(void) : pov(), dir(), camDir(), dist(0), light(), color() {}

Ray::~Ray(void) {}

Ray::Ray(const Ray& other) :
pov(other.pov),
dir(other.dir),
camDir(other.camDir),
norm(other.norm),
dist(other.dist),
light(other.light),
color(other.color)
{}

Ray& Ray::operator=(const Ray& other) {
	if (this != & other) {
		pov = other.pov;
		dir = other.dir;
		camDir = other.camDir;
		norm = other.norm;
		dist = other.dist;
		light = other.light;
		color = other.color;
	}
	return *this;
}

Ray& Ray::changePov(void) {
	pov.addition( pov, dir * dist );
	return *this;
}

Ray& Ray::movePovByEpsilon(void) {
	pov.addition(pov, norm * EPSILON);
	return *this;
}

Ray& Ray::reflect(const Ray& other) {
	*this = other;
	dir.reflect(norm, dir);
	movePovByEpsilon();
	return *this;
}

Ray& Ray::collectLight(const ARGBColor& objColor, const ARGBColor& light) {
	this->light = light;
	collectLight(objColor);
	return *this;
}

Ray& Ray::collectLight(const ARGBColor& objColor) {
	light.product(light, objColor);
	color.addition(color, light);
	return *this;
}

Ray& Ray::collectSpecularLight(const ARGBColor& objColor, int specular) {
	if (specular != -1) {
		dir.reflect(norm, dir);
		float k = dir * camDir;
		if (k > 0) {
			k = std::pow(k, specular);
			collectLight(objColor, light.product(k));
		}
	}
	return *this;
}

Ray& Ray::collectReflectiveLight(ARGBColor& reflectColor, float reflective) {
	color.addition(color.product(1 - reflective), reflectColor.product(reflective));
	return *this;
}
