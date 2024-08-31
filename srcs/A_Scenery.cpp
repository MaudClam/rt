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
d1{0,0,0},
d2{0,0,0},
s1(scenery),
s2(NULL),
hit(hit),
h1(hit),
h2(hit),
t1(scenery->combineType),
t2(END) {
	if ( s1->intersection(ray.set_hit(hit)) ) {
		d1[0] = ray.dist;
		d1[1] = ray.d1;
		d1[2] = ray.d2;
		h1 = ray.hit;
	} else {
		s1 = NULL;
	}
}

Combine::~Combine(void) {}

Combine::Combine(const Combine& other) :
ray(other.ray),
d1{other.d1[0],other.d1[1],other.d1[2]},
d2{other.d2[0],other.d2[1],other.d2[2]},
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
		for (int i = 0; i < 3; ++i) {
			d1[i] = other.d1[i];
			d2[i] = other.d2[i];
		}
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
	t2 = scenery->combineType;
	if ( s2->intersection(ray.set_hit(hit)) ) {
		d2[0] = ray.dist;
		d2[1] = ray.d1;
		d2[2] = ray.d2;
		h2 = ray.hit;
	} else {
		s2 = NULL;
	}
	chooseSurface();
	return *this;
}

void Combine::chooseSurface(void) {
	if (t1 == UNION) {
		t1 = t2;
		if (s1 && s2) {
			if (d1[0] > d2[0])
				visible2();
			else
				visible1();
		} else if (!s1 && !s2) {
			noVisible();
		} else if (!s1 && s2) {
			visible2();
		} else if (s1 && !s2) {
			visible1();
		}
	} else if (t1 == SUBTRACTION) {
		t1 = t2;
		if (s1 && s2) {
			subtraction12();
		} else if (!s1 && !s2) {
			noVisible();
		} else if (!s1 && s2) {
			noVisible();
		} else if (s1 && !s2) {
			visible1();
		}
	} else if (t1 == INTERSECTION) {
		t1 = t2;
		if (s1 && s2) {
			intersection();
		} else {
			noVisible();
		}
	}
}

void Combine::subtraction12(void) {
	if (_notIntersect()) {
		visible1();
	} else if (_1inside2()) {
		noVisible();
	} else if (_2inside1()) {
		if (h1 != INSIDE && h2 != INSIDE) {
			visible1();
		} else if (h1 == INSIDE && h2 == INSIDE) {
			visible2();
		} else if (h1 == INSIDE && h2 != INSIDE) {
			visible2();
		} else if (h1 != INSIDE && h2 == INSIDE) {
			visible1();
		}
	} else if (_12intrsect()) {
		if (h1 != INSIDE && h2 != INSIDE) {
			visible1();
		} else if (h1 == INSIDE && h2 == INSIDE) {
			noVisible();
		} else if (h1 == INSIDE && h2 != INSIDE) {
			visible2();
		} else if (h1 != INSIDE && h2 == INSIDE) {
			noVisible();
		}
	} else if (_21intrsect()) {
		if (h1 != INSIDE && h2 != INSIDE) {
			d2[0] = d2[2];
			h2 = INSIDE;
			visible2();
		} else if (h1 == INSIDE && h2 == INSIDE) {
			visible2();
		} else if (h1 != INSIDE && h2 == INSIDE) {
			visible2();
		} else if (h1 == INSIDE && h2 != INSIDE) {
			visible1();// not used in this case
		}
	}
}

void Combine::intersection(void) {
	if (d1[1] > d2[1]) {
		swap();
	}
	if (d1[1] > d2[1]) {
		swap();
	}
	if ( d2[1] >= 0 && d1[1] <= d2[1] && d2[1] <= d1[2] ) {
		d2[0] = d2[1];
		visible2();
	} else if ( d1[2] >= 0 && d2[1] <= d1[2] && d1[2] <= d2[2] ) {
		d1[0] = d1[2];
	} else if ( d2[2] >= 0 && d1[1] <= d2[2] && d2[2] <= d1[2] ) {
		d2[0] = d2[2];
		visible2();
	} else {
		noVisible();
	}
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
