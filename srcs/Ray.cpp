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
norm(),
dist(0),
light(),
shine(),
color(),
hit(FRONT)
{}

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
shine(other.shine),
color(other.color),
hit(other.hit)
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
		shine = other.shine;
		color = other.color;
		hit = other.hit;
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

Ray& Ray::partRestore(const Ray& other) {
//	recursion = other.recursion;
	pov = other.pov;
	dir = other.dir;
	dirFromCam = other.dirFromCam;
	dirToLight = other.dirToLight;
	norm = other.norm;
	dist = other.dist;
	light = other.light;
//	shine = other.shine;
//	color = other.color;
//	hit = other.hit;
	return *this;
}

Ray& Ray::movePovByNormal(float distance) {
	pov.addition(pov, norm * distance);
	return *this;
}

Ray& Ray::collectLight(const ARGBColor& sceneryColor, float k) {
	color.addition(color, light * sceneryColor * k);
	return *this;
}

Ray& Ray::collectShine(int specular) {
	if (specular != -1) {
		float k = dirToLight.reflect(norm) * dirFromCam;
		if (k > 0) {
			k = std::pow(k, specular);
			shine.addition(shine, light * k);
		}
	}
	return *this;
}

Ray& Ray::collectReflectiveLight(int _color, int _shine, float reflective) {
	float previous = 1. - reflective;
	light.val = _color;
	color.addition(color.product(reflective), light.product(previous));
	light.val = _shine;
	shine.addition(shine.product(reflective), light.product(previous));
	return *this;
}

Ray& Ray::collectRefractiveLight(const ARGBColor& sceneryColor, int _color, float refractive) {
	float previous = 1. - refractive;
	light.val = _color;
	color.addition(color.product(color,sceneryColor).product(refractive), light.product(previous));
	return *this;
}
