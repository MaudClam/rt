//
//  Ray.cpp
//  rt
//
//  Created by uru on 01/08/2024.
//

#include "Ray.hpp"


// struct RaySafe

RaySafe::RaySafe(void) :
pov(),
dir(),
dirFromCam(),
dirToLight(),
norm(),
dist(0)
{}

RaySafe::RaySafe(const Ray& ray) :
pov(ray.pov),
dir(ray.dir),
dirFromCam(ray.dirFromCam),
dirToLight(ray.dirToLight),
norm(ray.norm),
dist(ray.dist)
{}

RaySafe::~RaySafe(void) {}

RaySafe::RaySafe(const RaySafe& other) :
pov(other.pov),
dir(other.dir),
dirFromCam(other.dirFromCam),
dirToLight(other.dirToLight),
norm(other.norm),
dist(other.dist)
{}

RaySafe& RaySafe::operator=(const RaySafe& other) {
	if (this != &other) {
		pov = other.pov;
		dir = other.dir;
		dirFromCam = other.dirFromCam;
		dirToLight = other.dirToLight;
		norm = other.norm;
		dist = other.dist;
	}
	return *this;
}

RaySafe& RaySafe::operator=(const Ray& ray) {
	pov = ray.pov;
	dir = ray.dir;
	dirFromCam = ray.dirFromCam;
	dirToLight = ray.dirToLight;
	norm = ray.norm;
	dist = ray.dist;
	return *this;
}


// sruct ColorsSafe

ColorsSafe::ColorsSafe(void) : light(0), shine(0), color(0){}

ColorsSafe::ColorsSafe(Ray& ray) :
light(ray.light.val),
shine(ray.shine.val),
color(ray.color.val) {
	ray.light = ray.shine = ray.color = 0;
}

ColorsSafe::~ColorsSafe(void) {}

ColorsSafe::ColorsSafe(const ColorsSafe& other) :
light(other.light),
shine(other.shine),
color(other.color)
{}

ColorsSafe& ColorsSafe::operator=(const ColorsSafe& other) {
	if (this != &other) {
		light = other.light;
		shine = other.shine;
		color = other.color;
	}
	return *this;
}


// struct Ray

Ray::Ray(void) :
recursion(0),
intersections(),
light(),
shine(),
color(),
hit(FRONT),
t1(END),
t2(END),
segments()
{}

Ray::~Ray(void) {}

Ray::Ray(const Ray& other) { *this = other; }

Ray& Ray::operator=(const Ray& other) {
	if (this != & other) {
		recursion = other.recursion;
		pov = other.pov;
		dir = other.dir;
		dirFromCam = other.dirFromCam;
		dirToLight = other.dirToLight;
		norm = other.norm;
		dist = other.dist;
		intersections = other.intersections;
		light = other.light;
		shine = other.shine;
		color = other.color;
		hit = other.hit;
		t1 = other.t1;
		t2 = other.t2;
		segments = other.segments;
	}
	return *this;
}

Ray& Ray::operator=(const RaySafe& raySafe) {
	pov = raySafe.pov;
	dir = raySafe.dir;
	dirFromCam = raySafe.dirFromCam;
	dirToLight = raySafe.dirToLight;
	norm = raySafe.norm;
	dist = raySafe.dist;
	return *this;
}

Ray& Ray::operator=(const ColorsSafe& colorsSafe) {
	light.val = colorsSafe.light;
	shine.val = colorsSafe.shine;
	color.val = colorsSafe.color;
	return *this;
}

Ray& Ray::restore(const RaySafe& raySafe) {
	*this = raySafe;
	return *this;
}

Ray& Ray::restore(const ColorsSafe& colorsSafe) {
	*this = colorsSafe;
	return *this;
}

Ray& Ray::set_hit(Hit hit) {
	this->hit = hit;
	return *this;
}

void Ray::combineStart(A_Scenery* scenery, Hit targetHit) {
	t1 = scenery->combineType;
	segments.clear();
	if ( scenery->intersection(this->set_hit(targetHit)) ) {
		intersections.activate(scenery);
		emplace(intersections);
	}
}

void Ray::combineNext(A_Scenery* scenery, Hit targetHit) {
	t2 = scenery->combineType;
	if ( scenery->intersection(this->set_hit(targetHit)) ) {
		intersections.activate(scenery);
	} else {
		intersections.activate(NULL);
	}
	if (t1 == UNION) {
		if (first() && second()) {
			combination(t1);
		} else if (!first() && !second()) {
			noVisible();
		} else if (!first() && second()) {
			secondVisible();
		} else if (first() && !second()) {
			firstVisible();
		}
	} else if (t1 == SUBTRACTION) {
		if (first() && second()) {
			combination(t1);
		} else if (!first() && !second()) {
			noVisible();
		} else if (!first() && second()) {
			noVisible();
		} else if (first() && !second()) {
			firstVisible();
		}
	} else if (t1 == INTERSECTION) {
		if (first() && second()) {
			combination(t1);
		} else {
			noVisible();
		}
	}
	t1 = t2;
}

void Ray::combination(CombineType type) {
	for (auto segment = segments.begin(), end = segments.end(); segment != end; ++segment) {
		if (segment->removed) {
			continue;
		}
		if (type == UNION) {
			union_(*segment, intersections);
		} else if (type == SUBTRACTION) {
			subtraction(*segment, intersections);
		} else if (type == INTERSECTION) {
			intersection(*segment, intersections);
		}
	}
	if (type == UNION) {
		emplace(intersections);
	}
}

void Ray::union_(Segment& segment1, Segment& segment2) {
	if (segment1.a.d > segment2.b.d || segment2.a.d > segment1.b.d) {
		return;
	}
	if (segment2.a.d > segment1.a.d) {
		segment2.a = segment1.a;
	}		
	if (segment2.b.d < segment1.b.d) {
		segment2.b = segment1.b;
	}
	segment1.removed = true;
}

void Ray::subtraction(Segment& segment1, Segment& segment2) {
	if (segment1.a.d > segment2.b.d || segment2.a.d > segment1.b.d) {
		return;
	}
	if (segment1.a.d < segment2.a.d) {
		emplace(segment1.a, segment2.a);
	}
	if (segment1.b.d > segment2.b.d) {
		segment1.a = segment2.b;
	} else {
		segment1.removed = true;
	}
}

void Ray::intersection(Segment& segment1, Segment& segment2) {
	if (segment1.a.d > segment2.b.d || segment2.a.d > segment1.b.d) {
		segment1.removed = true;
		return;
	}
	if (segment1.a.d < segment2.a.d) {
		segment1.a = segment2.a;
	}
	if (segment1.b.d > segment2.b.d) {
		segment1.b = segment2.b;
	}
}

