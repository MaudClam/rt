//
//  Ray.cpp
//  rt
//
//  Created by uru on 01/08/2024.
//

#include "Ray.hpp"


// struct RayBasic

RayBasic::RayBasic(void) :
pov(),
dir(),
pow(),
dirL(),
norm(),
dist(0),
hit(FRONT)
{}

RayBasic::RayBasic(const Ray& ray) :
pov(ray.pov),
dir(ray.dir),
pow(ray.pow),
dirL(ray.dirL),
norm(ray.norm),
dist(ray.dist),
hit(ray.hit)
{}

RayBasic::~RayBasic(void) {}

RayBasic::RayBasic(const RayBasic& other) :
pov(other.pov),
dir(other.dir),
pow(other.pow),
dirL(other.dirL),
norm(other.norm),
dist(other.dist),
hit(other.hit)
{}

RayBasic& RayBasic::operator=(const RayBasic& other) {
	if (this != &other) {
		pov = other.pov;
		dir = other.dir;
		pow = other.pow;
		dirL = other.dirL;
		norm = other.norm;
		dist = other.dist;
		hit = other.hit;
	}
	return *this;
}

RayBasic& RayBasic::operator=(const Ray& ray) {
	pov = ray.pov;
	dir = ray.dir;
	pow = ray.pow;
	dirL = ray.dirL;
	norm = ray.norm;
	dist = ray.dist;
	hit = ray.hit;
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
combineType(END),
segments(),
traces()
{}

Ray::~Ray(void) {}

Ray::Ray(const Ray& other) { *this = other; }

Ray& Ray::operator=(const Ray& other) {
	if (this != & other) {
		recursion = other.recursion;
		pov = other.pov;
		dir = other.dir;
		pow = other.pow;
		dirL = other.dirL;
		norm = other.norm;
		dist = other.dist;
		hit = other.hit;
		intersections = other.intersections;
		light = other.light;
		shine = other.shine;
		color = other.color;
		combineType = other.combineType;
		segments = other.segments;
		traces = other.traces;
	}
	return *this;
}

Ray& Ray::operator=(const RayBasic& raySafe) {
	pov = raySafe.pov;
	dir = raySafe.dir;
	pow = raySafe.pow;
	dirL = raySafe.dirL;
	norm = raySafe.norm;
	dist = raySafe.dist;
	hit = raySafe.hit;
	return *this;
}

Ray& Ray::operator=(const ColorsSafe& colorsSafe) {
	light.val = colorsSafe.light;
	shine.val = colorsSafe.shine;
	color.val = colorsSafe.color;
	return *this;
}

Ray& Ray::restore(const RayBasic& raySafe) {
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

A_Scenery* Ray::closestScenery(a_scenerys_t& scenerys, float maxDistance, Hit target) {
	float		_distance = maxDistance;
	Hit			_hit = target;
	A_Scenery*	_closest = NULL;
	segments.clear();
	for (auto scenery = scenerys.begin(), end = scenerys.end(); scenery != end; ++scenery) {
		if ( (*scenery)->combineType == END ) {
			if ( (*scenery)->intersection(set_hit(target)) && _distance > dist ) {
				_closest = *scenery;
				_distance = dist;
				_hit = hit;
				if (target == FIRST_SHADOW) {
					intersections.activate(*scenery);
					emplace(intersections, false);
				}
			}
		} else {
			A_Scenery* _combine = combine(scenery, end, _distance, target);
			if (_combine) {
				_closest = _combine;
				_distance = dist;
				_hit = hit;
			}
		}
		if (target == ANY_SHADOW && _closest) {
			dist = _distance;
			hit = _hit;
			return _closest;
		}
	}
	if (_closest) {
		dist = _distance;
		hit = _hit;
		return _closest;
	}
	return NULL;
}

A_Scenery* Ray::combine(a_scenerys_it& scenery, a_scenerys_it& end, float distance, Hit target) {
	for (; scenery != end; ++scenery) {
		if ( (*scenery)->intersection(set_hit(target)) ) {
			intersections.activate(*scenery);
		} else {
			intersections.activate(NULL);
		}
		if (combineType == END) {				// first element
			combineType = (*scenery)->combineType;
			emplace(intersections, true);
		} else {								// each next element
			combination();
			combineType = (*scenery)->combineType;
		}
		if ( (*scenery)->combineType == END ) {	// last element
			combineType = END;
			return getCombine(intersections.a.set(distance, false, NULL));
		}
	}
	return NULL;
}

A_Scenery* Ray::getCombine(Point& nearest) {
	auto segment = segments.before_begin(), segmentNext = segments.begin();
	for (; segment != segments.end(); ++segment) {
		segmentNext = segment; segmentNext++;
		while (segmentNext != segments.end() && segmentNext->removed) {
			segmentNext = segments.erase_after(segment);
		}
		if (segmentNext != segments.end() && segmentNext->combine) {
			if (nearest.d > segmentNext->a.d && segmentNext->a.d >= 0) {
				nearest = segmentNext->a;
			}
			if (nearest.d > segmentNext->b.d && segmentNext->b.d >= 0) {
				nearest = segmentNext->b;
			}
			segmentNext->combine = false;
		}
	}
//	for (auto segment = segments.begin(), end = segments.end(); segment != end; ++segment) {
//		if (!segment->removed && segment->combine) {
//			if (nearest.d > segment->a.d && segment->a.d >= 0) {
//				nearest = segment->a;
//			}
//			if (nearest.d > segment->b.d && segment->b.d >= 0) {
//				nearest = segment->b;
//			}
//			segment->combine = false;
//		}
//	}
	if (nearest.s) {
		dist = nearest.d;
		hit = nearest.inside ? INSIDE : OUTSIDE;
	}
	return nearest.s;
}

void Ray::combination(void) {
	for (auto segment = segments.begin(), end = segments.end(); segment != end; ++segment) {
		if (segment->removed || !segment->combine) {
			continue;
		}
		switch (combineType) {
			case UNION:			union_(*segment, intersections); break;
			case SUBTRACTION:	subtraction(*segment, intersections); break;
			case INTERSECTION:	intersection(*segment, intersections); break;
			default: break;
		}
	}
	if (combineType == UNION && !intersections.empty()) {
		emplace(intersections, true);
	}
}

void Ray::union_(Segment& segment1, Segment& segment2) {
//	the result of the operation is placed in segment2
	if (!segment1.empty() && !segment2.empty()) {
		if (segment1.a.d > segment2.b.d || segment2.a.d > segment1.b.d) {
			return;
		}
		if (segment2.a.d > segment1.a.d) {
			segment2.a = segment1.a;
		}
		if (segment2.b.d < segment1.b.d) {
			segment2.b = segment1.b;
		}
//	} else if (segment1.empty() && !segment2.empty()) {	// ∅ ∪ a2 = a2
//		segment1.removed = true;
	} else if (!segment1.empty() && segment2.empty()) {	// a1 ∪ ∅ = a1
		segment2 = segment1;
//	} else if (segment1.empty() && segment2.empty()) {	// ∅ ∪ ∅ = ∅
//		segment1.removed = true;
	}
	segment1.removed = true;
}

void Ray::subtraction(Segment& segment1, Segment& segment2) {
//	the result of the operation is placed in segment1
	if (!segment1.empty() && !segment2.empty()) {
		if (segment1.a.d > segment2.b.d || segment2.a.d > segment1.b.d) {
			return;
		}
		if (segment1.a.d < segment2.a.d) {
			emplace(segment1.a, segment2.a, true);
		}
		if (segment1.b.d > segment2.b.d) {
			segment1.a = segment2.b;
		} else {
			segment1.removed = true;
		}
	} else if (segment1.empty() && !segment2.empty()) {	// ∅ – a2 = ∅
		segment1.removed = true;
//	} else if (!segment1.empty() && segment2.empty()) {	// a1 – ∅ = a1
//		nothing
	} else if (segment1.empty() && segment2.empty()) {	// ∅ – ∅ = ∅
		segment1.removed = true;
	}
}

void Ray::intersection(Segment& segment1, Segment& segment2) {
//	the result of the operation is placed in segment1
//	∅ ∩ a2 = ∅; a1 ∩ ∅ = ∅; ∅ ∩ ∅ = ∅;
	if (!segment1.empty() && !segment2.empty()) {
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
	} else {
		segment1.removed = true;
	}
}


// Non member functions

bool operator<(const Ray::Segment& left, const Ray::Segment& right) {
	return left.b.d < right.b.d;
}

std::ostream& operator<<(std::ostream& o, const Ray::Point& p) {
	o << "(" << p.d << ", " << std::boolalpha << p.inside << ", " << p.s << ")";
	return o;
}

std::ostream& operator<<(std::ostream& o, const Ray::Segment& s) {
	o << "a" << s.a << ", b" << s.b << ", " << std::boolalpha << s.removed << ", " << s.combine;
	return o;
}

std::ostream& operator<<(std::ostream& o, const Ray::segments_t& ss) {
	for (auto segment = ss.begin(), end = ss.end(); segment != end; ++segment) {
		o << *segment << std::endl;
	}
	if (!ss.empty()) {
		o << std::endl;
	}
	return o;
}
