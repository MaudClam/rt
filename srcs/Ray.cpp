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

// sruct ColorSafe

ColorSafe::ColorSafe(Ray& ray) : shine(ray.shine.val), color(ray.color.val) {
	ray.shine.val = ray.color.val = 0;
}

ColorSafe::~ColorSafe(void) {}

ColorSafe::ColorSafe(const ColorSafe& other) :
shine(other.shine),
color(other.color)
{}

ColorSafe& ColorSafe::operator=(const ColorSafe& other) {
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

Ray::Ray(rand_gen_t& gen, const Position pos, const Power& _pow) :
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
	randomUniformDirectionInHemisphere(gen, pos.n);
}

Ray::Ray(rand_gen_t& gen, const Position pos, const Power& _pow, const LookatAux& aux) :
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
	randomCosineWeightedDirectionInHemisphere(gen, aux);
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

Ray& Ray::operator=(const HitRecord& record) {
	pov = record.pov;
	dir = record.dir;
	norm = record.norm;
	hit = record.hit;
	scnr = record.scnr;
	return *this;
}

Ray& Ray::operator=(const ColorSafe& colorSafe) {
	shine.val = colorSafe.shine;
	color.val = colorSafe.color;
	return *this;
}

Ray& Ray::restore(const HitRecord& record) {
	*this = record;
	return *this;
}

Ray& Ray::restore(const ColorSafe& colorSafe) {
	*this = colorSafe;
	return *this;
}

Ray& Ray::restore(const HitRecord& record, const ColorSafe& colorSafe) {
	restore(record);
	restore(colorSafe);
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

bool Ray::closestScenery(a_scenerys_t& scenerys, float maxDistance, Hit target) {
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

Ray& Ray::combine(a_scenerys_it& scenery, a_scenerys_it& end, float distance, Hit target) {
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

Ray& Ray::ambientLiting(const HitRecord& record, const ARGBColor& ambient) {
	float k = record.norm * record.dir * -1;
	if (k > 0)
		collectLight(record.scnr->get_iColor(record), ambient, k);
	return *this;
}

Ray& Ray::directLitings(const HitRecord& record, a_scenerys_t& scenerys, a_scenerys_t& lightsIdx) {
	movePovByNormal(EPSILON);
	for (auto light = lightsIdx.begin(), end = lightsIdx.end(); light != end; ++light) {
		float k = (*light)->lighting(*this);
		if (k > 0) {
			if (!closestScenery(scenerys, dist, ANY_SHADOW)) {
				collectLight(record.scnr->get_iColor(record), k);
				collectShine(record.dir, record.scnr->specular);
			}
		}
	}
	restore(record);
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


// Non member functions

bool operator<(const Ray::Segment& left, const Ray::Segment& right) {
	return left.b.d < right.b.d;
}

