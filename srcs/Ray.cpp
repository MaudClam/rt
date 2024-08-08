//
//  Ray.cpp
//  rt
//
//  Created by uru on 01/08/2024.
//

#include "Ray.hpp"

// struct Ray

Ray::Ray(void) :
recursion(0),
pov(),
dir(),
dirFromCam(),
dirToLight(),
dist(0),
light(),
color() {}

Ray::~Ray(void) {}

Ray::Ray(const Ray& other) :
recursion(other.recursion),
pov(other.pov),
dir(other.dir),
dirFromCam(other.dirFromCam),
dirToLight(other.dirToLight),
norm(other.norm),
dist(other.dist),
light(other.light),
color(other.color)
{}

Ray& Ray::operator=(const Ray& other) {
	if (this != & other) {
		recursion = other.recursion;
		pov = other.pov;
		dir = other.dir;
		dirFromCam = other.dirFromCam;
		dirToLight = other.dirToLight;
		norm = other.norm;
		dist = other.dist;
		light = other.light;
		color = other.color;
	}
	return *this;
}

Ray& Ray::changePov(void) {
	pov.addition( pov, dir * dist );
	if (!recursion) {
		dirFromCam = dir;
	}
	return *this;
}

Ray& Ray::movePovByEpsilon(void) {
	pov.addition(pov, norm * EPSILON);
	return *this;
}

Ray& Ray::reflect(void) {
	dir.reflect(norm);
	recursion++;
	color.val = 0;
	movePovByEpsilon();
	return *this;
}

Ray& Ray::collectLight(const ARGBColor& sceneryColor, const ARGBColor& lightSource, float k) {
	light = lightSource;
	if (k != 1) {
		light.product(k);
	}
	light.product(light, sceneryColor);
	color.addition(color, light);
	return *this;
}

Ray& Ray::collectSpecular(const ARGBColor& sceneryColor, const ARGBColor& lightSource, int specular) {
	if (specular != -1) {
		dirToLight.reflect(norm);
		float k = dirToLight * dirFromCam;
		if (k > 0) {
			k = std::pow(k, specular);
			collectLight(sceneryColor, lightSource, k);
		}
	}
	return *this;
}
