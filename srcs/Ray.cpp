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

HitRecord::HitRecord(const Rgb& _pow, const Vec3f& _pov) :
pow(_pow),
pov(_pov),
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

HitRecord::HitRecord(Ray& ray, bool photon) :
paint(),
pov(ray.pov),
dir(ray.dir),
norm(ray.norm),
hit(ray.hit),
scnr(ray.scnr)
{
	if (photon)
		paint = ray.paint;
	else
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


// Struct PhotonTrace

PhotonTrace::PhotonTrace(void) : type(GLOBAL), pos(), pow(), scnrId(0) {}

PhotonTrace::PhotonTrace(MapType _type, const HitRecord& rec) :
type(_type),
pos(rec.pov, rec.dir),
pow(rec.pow),
scnrId(rec.scnr->get_id())
{}

PhotonTrace::PhotonTrace(const PhotonTrace& other) :
type(other.type),
pos(other.pos),
pow(other.pow),
scnrId(other.scnrId)
{}

PhotonTrace::~PhotonTrace(void) {}

PhotonTrace& PhotonTrace::operator=(const PhotonTrace& other) {
	if (this != &other) {
		type = other.type;
		pos = other.pos;
		pow = other.pow;
		scnrId = other.scnrId;
	}
	return *this;
}


// struct Ray

Ray::Ray(void) :
HitRecord(),
recursion(0),
dist(0),
path(),
intersections(),
combineType(END),
segments(),
traces()
{}

Ray::Ray(const Rgb& pow, const Vec3f& pov, int r) :
HitRecord(pow, pov),
recursion(r),
dist(0),
path(),
intersections(),
combineType(END),
segments(),
traces()
{}

Ray::Ray(const Position pos, const Rgb& _pow) :
HitRecord(),
recursion(1),
dist(0),
path(),
intersections(),
combineType(END),
segments(),
traces()
{
	pow = _pow;
	pov = pos.p;
	dir.randomInUnitHemisphere(pos.n);
}

Ray::Ray(const Position pos, const Rgb& _pow, const LookatAux& aux) :
HitRecord(),
recursion(1),
dist(0),
path(),
intersections(),
combineType(END),
segments(),
traces()
{
	pow = _pow;
	pov = pos.p;
	dir.randomInUnitHemisphereCosineDistribution(aux);
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

Ray& Ray::restore(const HitRecord& rec, bool part) {
	pov = rec.pov;
	dir = rec.dir;
	norm = rec.norm;
	if (!part) {
		paint.reset();
		hit = rec.hit;
		scnr = rec.scnr;
	}
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
		path.mark(DIFFUSION);
	return *this;
}

Ray& Ray::fakeAmbientLighting(HitRecord& rec, const Rgb& ambient) {
	float diffusion = rec.scnr->diffusion;
	if (diffusion) {
		float fading = -(rec.norm * rec.dir) * diffusion;
		if (fading > +0) {
			paint = ambient;
			paint.attenuate(rec.scnr->get_iColor(rec), fading);
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
				if (!closestScenery(scenerys, dist - 2 * EPSILON, ANY_SHADOW)) {
					if (glossy) {
						float shining = getShining_(rec.dir, rec.norm, dir, glossy);
						if (shining > _1_255)
							rec.paint += (paint * shining);
					}
					if ((lighting *= diffusion) > 0)
						rec.paint += paint.attenuate(rec.scnr->get_iColor(rec), lighting);
				}
			}
			restore(rec);
		}
	}
	return *this;
}

Ray& Ray::phMapLightings(HitRecord& rec, const PhotonMap& phMap, MapType type) {
	float diffusion = rec.scnr->diffusion;
	if (diffusion) {
		phMap.get_traces27(pov, traces, type);
		if (!traces.empty()) {
			std::map<float, TraceAround> sortedTraces;
			float sqRadius = phMap.get_sqr();
			int scnrId = rec.scnr->get_id(), estimate = phMap.estimate;
			for (auto trace = traces.begin(), end = traces.end(); trace != end; ++trace) {
				if ((*trace)->scnrId == scnrId) {
					float sqDistance = std::abs((rec.pov - (*trace)->pos.p).sqnorm());
					if (sqDistance <= sqRadius) {
						float fading = deNaN(-((*trace)->pos.n * rec.norm));
						if (fading > +0)
							sortedTraces.emplace(sqDistance, TraceAround(fading, *trace));
					}
				}
			}
			if (!traces.empty()) traces.clear();
			int n = 0;
			auto it = sortedTraces.begin(), last = sortedTraces.begin(), End = sortedTraces.end();
			for (; it != End && n <= estimate; ++it, n++) {
				pow += Rgb(it->second.trace->pow).attenuate(-1, it->second.fading);
				last = it;
			}
			if (!pow.isNull() && n >= MIN_ESTIMATE_PHOTONS) {
				pow *= float(1.0f / (M_PI * last->first) * n);
				pow.attenuate(rec.scnr->get_iColor(rec), diffusion);
				reset(rec);
			} else {
				restore(rec);
			}
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
	if (this->scnr->isGlowing(*this)) {
		return true;
	}
	markPath();
	return false;
}

bool Ray::photonEnd(const Scenerys& scenerys, int r) {
	if ((recursion = r) > MAX_PHOTON_COLLISIONS)
		return true;
	if (!closestScenery(scenerys, _INFINITY))
		return true;
	if (scnr->get_isLight())
		return true;
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

Choice Ray::chooseDirection(const HitRecord& rec, const Probability& p) {
	Choice choice = ABSORPTION;
	float mattness = scnr->get_mattness();
	double	chance = random_double();
	if (chance < p.refl()) {
		choice = REFLECTION;
		reflect(mattness);
	} else if (chance < p.refr()) {
		choice = REFRACTION;
		float schlick = mattness >= MATTNESS_GLOSSY_LIMIT ? -1 : 0;
		if (refract(hit == INSIDE ? scnr->matIOR : scnr->matOIR, schlick, mattness)) {
			if (random_double() < schlick) {
				choice = PARTIAL_REFLECTION;
				restore(rec, true);
				reflect(mattness);
			}
		} else {
			choice = FULL_REFLECTION;
			reflect(mattness);
		}
	} else if (chance < p.diff()) {
		if (hit == IN_VOLUME) {
			choice = DIFFUSION_IN_VOLUME;
			diffusionInVolume();
		} else {
			choice = DIFFUSION;
			diffusion();
		}
	}
	return choice;
}

int Ray::getAttenuation(HitRecord& rec, Choice choice, float& fading, float& shining) {
	int attenuation = 0;
	switch (choice) {
		case ABSORPTION: {
			attenuation = 0;
			fading = 0;
			shining = 0;
			break;
		}
		case REFLECTION: {
			attenuation = rec.scnr->diffusion ? -1 : rec.scnr->get_iColor(rec);
			fading = 1;
			shining = 0;
			break;
		}
		case PARTIAL_REFLECTION: {
			attenuation = rec.scnr->diffusion ? -1 : rec.scnr->get_iColor(rec);
			fading = 1;
			shining = 0;
			break;
		}
		case FULL_REFLECTION: {
			attenuation = rec.scnr->diffusion ? -1 : rec.scnr->get_iColor(rec);
			fading = 1;
			shining = 0;
			break;
		}
		case REFRACTION: {
			attenuation = rec.scnr->get_iColor(rec);
			fading = 1;
			shining = 0;
			break;
		}
		case DIFFUSION: {
			attenuation = rec.scnr->get_iColor(rec);
			fading *= (dir * rec.norm);
			float glossy = rec.scnr->get_glossy();
			if (glossy)
				shining = getShining(rec.dir, rec.norm, dir, glossy);
			break;
		}
		case DIFFUSION_IN_VOLUME: {
			attenuation = rec.scnr->get_iColor(rec);
			fading *= -(dir * rec.dir);
			fading = fading > +0 ? fading : 0;
			break;
		}
		default: break;
	}
	return attenuation;
}


// struct Rays

Rays::Rays(void) : std::vector<Ray>() {}

Rays::~Rays(void) {}

Rays& Rays::createPhotons(int amt, const Rgb& pow, const Vec3f& pov, const Vec3f& normal, Distribution distr) {
	LookatAux aux(normal);
	for (int i = 0; i < amt; i++) {
		emplace_back(pow, pov);
		switch (distr) {
			case DIRECT:			back().dir = normal; break;
			case SPHERE:			back().dir.randomInUnitSphere(); break;
			case HEMISPHERE:		back().dir.randomInUnitHemisphere(normal); break;
			case HEMISPHERE_COSINE:	back().dir.randomInUnitHemisphereCosineDistribution(aux); break;
			default: break;
		}
	}
	return *this;
}

Rays& Rays::clear_(int n) {
	Rays tmp;
	if (n) tmp.reserve(n);
	swap(tmp);
	return *this;
}


// Non member functions

bool operator<(const Ray::Segment& left, const Ray::Segment& right) {
	return left.b.d < right.b.d;
}

