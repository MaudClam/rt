//
//  Ray.cpp
//  rt
//
//  Created by uru on 05/07/2024.
//

#include "Ray.hpp"

Ray::Ray(void) :
addr(NULL),
bytespp(0),
pos(),
tan(1.),
pov(),
dir(0,0,1.),
dist(INFINITY),
color(),
generation(0)
{}

Ray::~Ray(void) {}

Ray::Ray(void* addr, int bytespp, const Vec2f& pos, float tan) :
addr(addr),
bytespp(bytespp),
pos(pos),
tan(tan),
pov(),
dir(pos.x,pos.y,1.),
dist(INFINITY),
color(),
generation(0) {
	if (tan != 1.) {
		dir.x *= tan;
		dir.y *= tan;;
	}
	dir.normalize();
}

Ray::Ray(const Ray& other) :
addr(other.addr),
bytespp(other.bytespp),
pos(other.pos),
tan(other.tan),
pov(other.pov),
dir(other.dir),
dist(other.dist),
color(other.color),
generation(other.generation) {}

Ray& Ray::operator=(const Ray& other) {
	if (this != &other) {
		addr = other.addr;
		bytespp = other.bytespp;
		pos = other.pos;
		tan = other.tan;
		pov = other.pov;
		dir = other.dir;
		dist = other.dist;
		color = other.color;
		generation = other.generation;
	}
	return *this;
}

void Ray::reset(float tan) {
	this->tan = tan;
	pov.x = pov.y = pov.z = 0;
	dir.x = pos.x * tan; dir.y = pos.y * tan; dir.z = 1.;
	dir.normalize();
	dist = INFINITY;
	color.val = 0; color.bytespp = RGB;
}

void Ray::drawPixel(void) {
	memcpy(addr, color.raw, bytespp);
}
