#include "Ray.hpp"


// Class PhotonPath

PhotonPath::PhotonPath(void) : r(false), d(false), v(false) {}

PhotonPath::~PhotonPath(void) {}

PhotonPath::PhotonPath(const PhotonPath& other) :
r(other.r),
d(other.d),
v(other.d)
{}

PhotonPath& PhotonPath::operator=(const PhotonPath& other){
	if (this != &other) {
		r = other.r;
		d = other.d;
		v = other.v;
	}
	return *this;
}


// struct RayBasic

RayBasic::RayBasic(void) :
pov(),
dir(),
dirС(),
dirL(),
norm(),
dist(0),
hit(FRONT)
{}


RayBasic::~RayBasic(void) {}

RayBasic::RayBasic(const RayBasic& other) :
pov(other.pov),
dir(other.dir),
dirС(other.dirС),
dirL(other.dirL),
norm(other.norm),
dist(other.dist),
hit(other.hit)
{}

RayBasic& RayBasic::operator=(const RayBasic& other) {
	if (this != &other) {
		pov = other.pov;
		dir = other.dir;
		dirС = other.dirС;
		dirL = other.dirL;
		norm = other.norm;
		dist = other.dist;
		hit = other.hit;
	}
	return *this;
}

// sruct ColorsSafe

ColorsSafe::ColorsSafe(Ray& ray) : light(ray.light.val), shine(ray.shine.val), color(ray.color.val) {
	ray.light.val = ray.shine.val = ray.color.val = 0;
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
pow(),
path(),
intersections(),
light(),
shine(),
color(),
combineType(END),
segments(),
traces()
{}

Ray::Ray(rand_gen_t& gen, const Position pos, const Power& _pow) :
recursion(1),
pow(_pow),
path(),
intersections(),
light(),
shine(),
color(),
combineType(END),
segments(),
traces()
{
	pov = pos.p;
	randomUniformDirectionInHemisphere(gen, pos.n);
}

Ray::Ray(rand_gen_t& gen, const Position pos, const Power& _pow, const LookatAux& aux) :
recursion(1),
pow(_pow),
path(),
intersections(),
light(),
shine(),
color(),
combineType(END),
segments(),
traces()
{
	pov = pos.p;
	randomCosineWeightedDirectionInHemisphere(gen, aux);
}

Ray::~Ray(void) {}

Ray::Ray(const Ray& other) { *this = other; }

Ray& Ray::operator=(const Ray& other) {
	if (this != & other) {
		pov = other.pov;
		dir = other.dir;
		dirС = other.dirС;
		dirL = other.dirL;
		norm = other.norm;
		dist = other.dist;
		hit = other.hit;
		recursion = other.recursion;
		pow = other.pow;
		path = other.path;
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
	dirС = raySafe.dirС;
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

Ray& Ray::getRayBasic(RayBasic& rayBasic) {
	rayBasic.pov = pov;
	rayBasic.dir = dir;
	rayBasic.dirС = dirС;
	rayBasic.dirL = dirL;
	rayBasic.norm = norm;
	rayBasic.dist = dist;
	rayBasic.hit = hit;
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

Ray& Ray::restore(const RayBasic& raySafe, const ColorsSafe& colorsSafe) {
	restore(raySafe);
	restore(colorsSafe);
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

