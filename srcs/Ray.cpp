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
segments(),
del()
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
		del = other.del;
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

Ray& Ray::set_hit(Hit hit) {
	this->hit = hit;
	return *this;
}

void Ray::emplace(const Segment& segment) {
	segments.emplace_front(segment.a.d, segment.a.inside, segment.a.s,
						   segment.b.d, segment.b.inside, segment.b.s );
}

void Ray::emplace(const Point& a, const Point& b) {
	segments.emplace_front(a.d, a.inside, a.s, b.d, b.inside, b.s );
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
		bool _del = false;
		if (type == UNION) {
			_del = union_(*segment, intersections);
		} else if (type == SUBTRACTION) {
			_del = subtraction(*segment, intersections);
		} else if (type == INTERSECTION) {
			_del = intersection(*segment, intersections);
		}
		if (_del) {
			del.push_back(segment);
		}
	}
	if (type == UNION) {
		emplace(intersections);
	}
	for (auto it = del.begin(), end = del.end(); it != end; ++it) {
		segments.erase(*it);
	}
	del.clear();
}

bool Ray::union_(Segment& segment1, Segment& segment2) {
	if (ab1_intersect_ab2(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment2.a = segment1.a;
		return true;
	} else if (ab2_intersect_ab1(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment2.b = segment1.b;
		return true;
	} else if (ab1_inside_ab2(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		return true;
	} else if (ab2_inside_ab1(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment2.a = segment1.a;
		segment2.b = segment1.b;
		return true;
	} else if (equal(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		return true;
	}
	return false;
}

bool Ray::subtraction(Segment& segment1, Segment& segment2) {
	if (ab1_intersect_ab2(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment1.b = segment2.a;
	} else if (ab2_intersect_ab1(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment1.a = segment2.b;
	} else if (ab2_inside_ab1(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		if (segment1.a.d != segment2.a.d) {
			emplace(segment1.a, segment2.a);
		}
		if (segment1.b.d != segment2.b.d) {
			emplace(segment2.b, segment1.b);
		}
		return true;
	} else if (ab1_inside_ab2(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		return true;
	}
	return false;
}

bool Ray::intersection(Segment& segment1, Segment& segment2) {
	if (ab1_intersect_ab2(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment1.a = segment2.a;
		return false;
	} else if (ab2_intersect_ab1(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment1.b = segment2.b;
		return false;
	} else if (ab1_inside_ab2(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		return false;
	} else if (ab2_inside_ab1(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment1 = segment2;
		return false;
	}
	return true;
}


Ray& Ray::changePov(void) {
	pov.addition( pov, dir * dist );
	if (!recursion) {
		dirFromCam = dir;
	}
	return *this;
}

Ray& Ray::partRestore(const Ray& other) {//FIXME
//	recursion = other.recursion;
	pov = other.pov;
	dir = other.dir;
	dirFromCam = other.dirFromCam;
	dirToLight = other.dirToLight;
	norm = other.norm;
	dist = other.dist;
//	light = other.light;
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
