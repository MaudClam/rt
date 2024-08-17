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
_pov(),
dir(),
_dir(),
dirFromCam(),
dirToLight(),
norm(),
_norm(),
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
_pov(other._pov),
dir(other.dir),
_dir(other._dir),
dirFromCam(other.dirFromCam),
dirToLight(other.dirToLight),
norm(other.norm),
_norm(other._norm),
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
		_pov = other._pov;
		dir = other.dir;
		_dir = other._dir;
		dirFromCam = other.dirFromCam;
		dirToLight = other.dirToLight;
		norm = other.norm;
		_norm = other._norm;
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

Ray& Ray::saveHitPosition(void) {
	_pov = pov;
	_dir = dir;
	_norm = norm;
	return *this;
}

Ray& Ray::restoreHitPosition(void) {
	pov = _pov;
	dir = _dir;
	norm = _norm;
	return *this;
}

Ray& Ray::movePovByNormal(float distance) {
	pov.addition(pov, norm * distance);
	return *this;
}

Ray& Ray::reflect(void) {
	dir.reflect(norm);
	movePovByNormal(EPSILON);
	color = 0;
	shine = 0;
	return *this;
}

Ray& Ray::collectLight(const ARGBColor& sceneryColor, const ARGBColor& lightSource, float k) {
	color.addition(color, lightSource * sceneryColor * k);
	return *this;
}

Ray& Ray::collectShine(const ARGBColor& sceneryColor, const ARGBColor& lightSource, int specular) {
	(void)sceneryColor;
	if (specular != -1) {
		float k = dirToLight.reflect(norm) * dirFromCam;
		if (k > 0) {
			k = std::pow(k, specular);
			shine.addition(shine, lightSource * k);
		}
	}
	return *this;
}

Ray& Ray::collectReflectiveLight(int _color, int _shine, float reflective) {
	float diffusion = 1. - reflective;
	light.val = _color;
	color.addition(color.product(reflective), light.product(diffusion));
	light.val = _shine;
	shine.addition(shine.product(reflective), light.product(diffusion));
	return *this;
}

Ray& Ray::collectRefractiveLight(const ARGBColor& sceneryColor, int _color, float refractive) {
	float reflective = 1. - refractive;
	light.val = _color;
	color.addition(color.product(color,sceneryColor).product(refractive), light.product(reflective));
	return *this;
}
