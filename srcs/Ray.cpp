#include "Ray.hpp"


// struct HitRecord

HitRecord::HitRecord(void) :
paint(),
pov(),
dir(),
norm(),
hit(FRONT),
scnr(NULL)
{}


HitRecord::~HitRecord(void) {}

HitRecord::HitRecord(const HitRecord& other) :
paint(other.paint),
pov(other.pov),
dir(other.dir),
norm(other.norm),
hit(other.hit),
scnr(other.scnr)
{}

HitRecord::HitRecord(Ray& ray) :
paint(),
pov(ray.pov),
dir(ray.dir),
norm(ray.norm),
hit(ray.hit),
scnr(ray.scnr)
{
	ray.paint.reset();
}

HitRecord& HitRecord::operator=(const HitRecord& other) {
	if (this != &other) {
		paint = other.paint;
		pov = other.pov;
		dir = other.dir;
		norm = other.norm;
		hit = other.hit;
		scnr = other.scnr;
	}
	return *this;
}


// struct Ray

Ray::Ray(void) :
HitRecord(),
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
HitRecord(),
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
HitRecord(),
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
		paint = other.paint;
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

Ray& Ray::operator=(const HitRecord& other) {
	if (this != & other) {
		paint = other.paint;
		pov = other.pov;
		dir = other.dir;
		norm = other.norm;
		hit = other.hit;
	}
	return *this;
}

Ray& Ray::reset(HitRecord& rec) {
	rec.paint += paint;
	restore(rec);
	return *this;
}

Ray& Ray::restore(const HitRecord& rec) {
	paint.reset();
	pov = rec.pov;
	dir = rec.dir;
	norm = rec.norm;
	hit = rec.hit;
	scnr = rec.scnr;
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

Ray& Ray::markPath(void) {
	if (scnr->diffusion || scnr->get_mattness())
		path.diffusion(true);
	return *this;
}

Ray& Ray::fakeAmbientLighting(HitRecord& rec, const Rgb& ambient) {
	float diffusion = rec.scnr->diffusion;
	if (diffusion) {
		float intensity = -(rec.norm * rec.dir) * diffusion;
		if (intensity > 0) {
			paint = ambient;
			paint.attenuate(rec.scnr->get_iColor(rec), intensity);
			reset(rec);
		}
	}
	return *this;
}

Ray& Ray::directLightings(HitRecord& rec, const Scenerys& scenerys, const Scenerys& lightsIdx) {
	float diffusion = rec.scnr->diffusion;
	float glossy = rec.scnr->get_glossy();
	if (diffusion || glossy) {
		for (auto lightSrc = lightsIdx.begin(); lightSrc != lightsIdx.end(); ++lightSrc) {
			float lighting = (*lightSrc)->lighting(*this);
			if (lighting > 0) {
				movePovByNormal(EPSILON);
				if (!closestScenery(scenerys, dist, ANY_SHADOW)) {
					if ((lighting *= diffusion) > 0) {
						paint = (*lightSrc)->light.light;
						rec.paint += paint.attenuate(rec.scnr->get_iColor(rec), lighting);
					}
					if (glossy) {
						float shining = getShining_(rec.dir, rec.norm, dir, glossy);
						if (shining > _1_255) {
							paint = (*lightSrc)->light.light;
							rec.paint += (paint *= shining);
						}
					}
				}
			}
			restore(rec);
		}
	}
	return *this;
}

Ray& Ray::phMapLightings(HitRecord& rec, const PhotonMap& phMap, MapType type) {
	if (rec.scnr->diffusion) {
		phMap.get_traces27(pov, traces, type);
		if (!traces.empty()) {
			phMapLightings(phMap.get_sqr(), phMap.estimate,
						   rec.scnr->get_id(), rec.scnr->get_iColor(rec));
			rec.paint += color.val;
			rec.paint += shine.val;
		}
	}
	return *this;
}

bool Ray::end(const Scenerys& scenerys, const Lighting& background, int depth, int r) {
	if ((recursion = r) > depth) {
		paint.reset();
		return true;
	}
	if (!closestScenery(scenerys, _INFINITY)) {
		paint = background.light;
		return true;
	}
	if (isGlowing()) {
		paint = scnr->get_iColor(*this);
		return true;
	}
	markPath();
	return false;
}

bool Ray::closestScenery(const Scenerys& scenerys, float maxDistance, Hit target) {
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

bool Ray::isGlowing(void) {
	if (scnr->get_isLight()) {
		color = scnr->get_iColor(*this);
		return true;
	}
	return false;
}

float Ray::probability(Choice choice, bool isPhoton) {
	if (!isPhoton) {
		switch (choice) {
			case REFLECTION: return scnr->reflective;
			case REFRACTION: return scnr->reflective + scnr->refractive;
			case DIFFUSION:  return scnr->reflective + scnr->refractive + scnr->diffusion;
			default: break;
		}
	}
	return ERROR;
}

Choice Ray::chooseDirection(HitRecord& rec, bool isPhoton) {
	float	mattness = scnr->get_mattness();
	double	chance = random_double();
	if (chance < probability(REFLECTION, isPhoton)) {
		reflect(mattness);
		return REFLECTION;
	} else if (chance < probability(REFRACTION, isPhoton)) {
		float schlick = mattness >= MATTNESS_GLOSSY_LIMIT ? -1 : 0;
		if (refract(hit == INSIDE ? scnr->matIOR : scnr->matOIR, schlick, mattness)) {
			if (random_double() < schlick) {
				restore(rec);
				reflect(mattness);
				return PARTIAL_REFLECTION;
			}
			return REFRACTION;
		} else {
			reflect(mattness);
			return FULL_REFLECTION;
		}
	} else if (chance < probability(DIFFUSION, isPhoton)) {
		diffusion();
		return DIFFUSION;
	}
	return ABSORPTION;
}

int Ray::getAttenuation(HitRecord& rec, Choice choice, float& intensity, float& shining) {
	int attenuation = 0;
	switch (choice) {
		case ABSORPTION: {
			attenuation = 0;
			intensity = 0;
			shining = 0;
			break;
		}
		case REFLECTION: {
			attenuation = rec.scnr->diffusion ? -1 : rec.scnr->get_iColor(rec);
			intensity = 1;
			shining = 0;
			break;
		}
		case PARTIAL_REFLECTION: {
			attenuation = rec.scnr->diffusion ? -1 : rec.scnr->get_iColor(rec);
			intensity = 1;
			shining = 0;
			break;
		}
		case FULL_REFLECTION: {
			attenuation = rec.scnr->diffusion ? -1 : rec.scnr->get_iColor(rec);
			intensity = 1;
			shining = 0;
			break;
		}
		case REFRACTION: {
			attenuation = rec.scnr->get_iColor(rec);
			intensity = 1;
			shining = 0;
			break;
		}
		case DIFFUSION: {
			attenuation = rec.scnr->get_iColor(rec);
			intensity = dir * rec.norm;
			shining = getShining(rec.dir, rec.norm, dir, rec.scnr->get_glossy());
			break;
		}
		default: break;
	}
	return attenuation;
}


// Non member functions

bool operator<(const Ray::Segment& left, const Ray::Segment& right) {
	return left.b.d < right.b.d;
}

