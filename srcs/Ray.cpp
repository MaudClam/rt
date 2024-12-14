#include "Ray.hpp"


// struct HitRecord

HitRecord::HitRecord(void) :
pov(),
dir(),
norm(),
hit(FRONT),
scnr(NULL)
{}


HitRecord::~HitRecord(void) {}

HitRecord::HitRecord(const HitRecord& other) :
pov(other.pov),
dir(other.dir),
norm(other.norm),
hit(other.hit),
scnr(other.scnr)
{}

HitRecord::HitRecord(const Ray& ray) :
pov(ray.pov),
dir(ray.dir),
norm(ray.norm),
hit(ray.hit),
scnr(ray.scnr)
{}

HitRecord& HitRecord::operator=(const HitRecord& other) {
	if (this != &other) {
		pov = other.pov;
		dir = other.dir;
		norm = other.norm;
		hit = other.hit;
		scnr = other.scnr;
	}
	return *this;
}

// sruct ColorRecord

ColorRecord::ColorRecord(Ray& ray) : shine(ray.shine.val), color(ray.color.val) {
	ray.shine.val = ray.color.val = 0;
}

ColorRecord::~ColorRecord(void) {}

ColorRecord::ColorRecord(const ColorRecord& other) :
shine(other.shine),
color(other.color)
{}

ColorRecord& ColorRecord::operator=(const ColorRecord& other) {
	if (this != &other) {
		shine = other.shine;
		color = other.color;
	}
	return *this;
}


// struct Ray

Ray::Ray(void) :
recursion(0),
dist(0),
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

Ray::Ray(const Position pos, const Power& _pow) :
recursion(1),
dist(0),
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
	randomUniformDirectionInHemisphere(pos.n);
}

Ray::Ray(const Position pos, const Power& _pow, const LookatAux& aux) :
recursion(1),
dist(0),
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
	randomCosineWeightedDirectionInHemisphere(aux);
}

Ray::~Ray(void) {}

Ray::Ray(const Ray& other) { *this = other; }

Ray& Ray::operator=(const Ray& other) {
	if (this != & other) {
		pov = other.pov;
		dir = other.dir;
		norm = other.norm;
		hit = other.hit;
		recursion = other.recursion;
		dist = other.dist;
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

Ray& Ray::operator=(const HitRecord& rec) {
	pov = rec.pov;
	dir = rec.dir;
	norm = rec.norm;
	hit = rec.hit;
	scnr = rec.scnr;
	return *this;
}

Ray& Ray::operator=(const ColorRecord& cRec) {
	shine.val = cRec.shine;
	color.val = cRec.color;
	return *this;
}

Ray& Ray::restore(const HitRecord& rec) {
	*this = rec;
	return *this;
}

Ray& Ray::restore(const ColorRecord& cRec) {
	*this = cRec;
	return *this;
}

Ray& Ray::restore(const HitRecord& rec, const ColorRecord& cRec) {
	restore(rec);
	restore(cRec);
	return *this;
}

Ray& Ray::set_hit(Hit hit) {
	this->hit = hit;
	return *this;
}

Ray& Ray::getNormal(void) {
	movePovByDirToDist();
	scnr->getNormal(*this);
	return *this;
}

Ray& Ray::combination(void) {
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
	return *this;
}

Ray& Ray::union_(Segment& segment1, Segment& segment2) {
//	the result of the operation is placed in segment2
	if (!segment1.empty() && !segment2.empty()) {
		if (segment1.a.d > segment2.b.d || segment2.a.d > segment1.b.d) {
			return *this;
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
	return *this;
}

Ray& Ray::subtraction(Segment& segment1, Segment& segment2) {
//	the result of the operation is placed in segment1
	if (!segment1.empty() && !segment2.empty()) {
		if (segment1.a.d > segment2.b.d || segment2.a.d > segment1.b.d) {
			return *this;
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
	return *this;
}

Ray& Ray::intersection(Segment& segment1, Segment& segment2) {
//	the result of the operation is placed in segment1
//	∅ ∩ a2 = ∅; a1 ∩ ∅ = ∅; ∅ ∩ ∅ = ∅;
	if (!segment1.empty() && !segment2.empty()) {
		if (segment1.a.d > segment2.b.d || segment2.a.d > segment1.b.d) {
			segment1.removed = true;
			return *this;
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
	return *this;
}

Ray& Ray::combine(auto& scenery, auto& end, float distance, Hit target) {
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
			scnr = getCombine(intersections.a.set(distance, false, NULL));
			return *this;
		}
	}
	scnr = NULL;
	return *this;
}

bool Ray::closestScenery(Scenerys& scenerys, float maxDistance, Hit target) {
	float		_distance = maxDistance;
	Hit			_hit = target;
	A_Scenery*	_closest = NULL;
	segments.clear_();
	for (auto scenery = scenerys.begin(), end = scenerys.end(); scenery != end; ++scenery) {
		if ( (*scenery)->combineType == END ) {
			if ( (*scenery)->intersection(set_hit(target)) && _distance > dist ) {
				_closest = *scenery;
				_distance = dist;
				_hit = hit;
				if (target == ALL_SHADOWS) {
					intersections.activate(*scenery);
					emplace(intersections, false);
				}
			}
		} else {
			combine(scenery, end, _distance, target);
			if (scnr) {
				_closest = scnr;
				_distance = dist;
				_hit = hit;
			}
		}
		if (target == ANY_SHADOW && _closest) {
			dist = _distance;
			hit = _hit;
			scnr = _closest;
			return true;
		}
	}
	if (_closest) {
		dist = _distance;
		hit = _hit;
		scnr = _closest;
		return true;
	}
	scnr = NULL;
	return false;
}


// Non member functions

bool operator<(const Ray::Segment& left, const Ray::Segment& right) {
	return left.b.d < right.b.d;
}

