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
d(1),
pov(),
dir(0,0,d),
dist(INFINITY),
color(),
generation(0) {}

Ray::~Ray(void) {}

Ray::Ray(void* addr, int bytespp, const Vec2f& pos, float d) :
addr(addr),
bytespp(bytespp),
pos(pos),
d(d),
pov(),
dir(pos.x,pos.y,d),
dist(INFINITY),
color(),
generation(0) {
	dir.normalize();
}

Ray::Ray(const Ray& other) :
addr(other.addr),
bytespp(other.bytespp),
pos(other.pos),
d(other.d),
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
		d = other.d;
		pov = other.pov;
		dir = other.dir;
		dist = other.dist;
		color = other.color;
		generation = other.generation;
	}
	return *this;
}

void Ray::reset(void) {
	pov.x = pov.y = pov.z = 0;
	dir.x = pos.x; dir.y = pos.y; dir.z = d;
	dir.normalize();
	dist = INFINITY;
	color.val = 0; color.bytespp = RGB;
}

void Ray::set_d(float d) {
	this->d = d;
	reset();
}

void Ray::drawPixel(void) {
	memcpy(addr, color.raw, bytespp);
}


// Non member functions

float d_calculate(int fov) {
	float d = 0.5 / std::tan(std::numbers::pi * fov / 360);
	return d;
}
