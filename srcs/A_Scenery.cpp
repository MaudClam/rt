//
//  A_Scenery.cpp
//  rt
//
//  Created by uru on 09/07/2024.
//

#include "A_Scenery.hpp"


// struct Combinator

Combine::Combine(Ray& ray, A_Scenery* scenery, Hit hit) :
ray(ray),
d1(0),
d2(0),
s1(scenery),
s2(NULL),
hit(hit),
h1(hit),
h2(hit),
t1(scenery->combineType),
t2(END) {
	if ( s1->intersection(ray.set_hit(hit)) ) {
		d1 = ray.dist;
		h1 = ray.hit;
	} else {
		s1 = NULL;
	}
}

Combine::~Combine(void) {}

Combine::Combine(const Combine& other) :
ray(other.ray),
d1(other.d1),
d2(other.d2),
s1(other.s1),
s2(other.s2),
hit(other.hit),
h1(other.h1),
h2(other.h2),
t1(other.t1),
t2(other.t2)
{}

Combine& Combine::operator=(const Combine& other) {
	if (this != &other) {
		ray = other.ray;
		d1 = other.d1;
		d2 = other.d2;
		s1 = other.s1;
		s2 = other.s2;
		hit = other.hit;
		h1 = other.h1;
		h2 = other.h2;
		t1 = other.t1;
		t2 = other.t2;
	}
	return *this;
}

Combine& Combine::nextPrimitive(A_Scenery* scenery) {
	s2 = scenery;
	if ( s2->intersection(ray.set_hit(hit)) ) {
		d2 = ray.dist;
		h2 = ray.hit;
	} else {
		s2 = NULL;
	}
	chooseSurface();
	return *this;
}

Combine& Combine::chooseSurface(void) {
	if ( std::abs(d2) == std::abs(combine(d1, d2, t1)) ) {
		d1 = d2;
		s1 = s2;
		h1 = h2;
	}
	t1 = t2;
	return *this;
}


// class A_Scenery

A_Scenery::A_Scenery(void) :
_name(),
_nick(),
_isLight(false),
_pos(),
combineType(END),
color(),
specular(-1),
reflective(0),
refractive(0),
a_matIOR(1)
{}

A_Scenery::~A_Scenery(void) {}

A_Scenery::A_Scenery(const A_Scenery& other) :
_name(other._name),
_nick(other._nick),
_isLight(other._isLight),
_pos(other._pos),
combineType(other.combineType),
color(other.color),
specular(other.specular),
reflective(other.reflective),
refractive(other.refractive),
a_matIOR(other.a_matIOR)
{}

std::string A_Scenery::get_nick(void) const { return _nick; }

bool A_Scenery::get_isLight(void) const { return _isLight; }

Position A_Scenery::get_pos(void) const { return _pos; }

void A_Scenery::set_pos(const Position& pos) { _pos = pos; }


// Non member functions

std::ostream& operator<<(std::ostream& o, A_Scenery& s) {
	std::ostringstream os;
	s.output(os);
	o << os.str();
	return o;
}
