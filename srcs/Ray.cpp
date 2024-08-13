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

Ray& Ray::movePovByEpsilon(void) {
	pov.addition(pov, norm * EPSILON);
	return *this;
}

Ray& Ray::reflect(void) {
	dir.reflect(norm);
	recursion++;
	movePovByEpsilon();
	color = 0;
	shine = 0;
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

Ray& Ray::collectShine(const ARGBColor& sceneryColor, const ARGBColor& lightSource, int specular) {
	if (specular != -1) {
		float k = dirToLight.reflect(norm) * dirFromCam;
		if (k > 0) {
			k = std::pow(k, specular);
			light = lightSource;
			light.product(k);
			light.product(light, sceneryColor);
			shine.addition(shine, light);
		}
	}
	return *this;
}

Ray& Ray::collectReflect(int _color, int _shine, float reflective) {
	float diffusion = 1 - reflective;
	light.val = _color;
	color.addition(color.product(reflective), light.product(diffusion));
	light.val = _shine;
	shine.addition(shine.product(reflective), light.product(diffusion));
	return *this;
}
