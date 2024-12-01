#include "PhotonMap.hpp"


// Struct Claster

ClasterKey::ClasterKey(void) : type(NO), x(0), y(0), z(0) {}

ClasterKey::ClasterKey(MapType _type, int _x, int _y, int _z) : type(_type), x(_x), y(_y), z(_z) {}

ClasterKey::ClasterKey(MapType type, const Vec3f point, float gridStep) : type(NO), x(0), y(0), z(0) {
	make(type, point, gridStep);
}

ClasterKey::ClasterKey(const PhotonTrace& trace, float gridStep) : type(NO), x(0), y(0), z(0) {
	make(trace, gridStep);
}

ClasterKey::ClasterKey(const ClasterKey& other) : type(other.type), x(other.x), y(other.y), z(other.z)   {}

ClasterKey::~ClasterKey(void) {}

ClasterKey& ClasterKey::operator=(const ClasterKey& other) {
	if (this != &other) {
		type = other.type;
		x = other.x;
		y = other.y;
		z = other.z;
	}
	return *this;
}

ClasterKey&	ClasterKey::make(MapType _type, const Vec3f& point, float gridStep) {
	type = _type;
	x = std::floor(point.x / gridStep);
	y = std::floor(point.y / gridStep);
	z = std::floor(point.z / gridStep);
	return *this;
}

ClasterKey&	ClasterKey::make(const PhotonTrace& trace, float gridStep) {
	return make(trace.type, trace.pos.p, gridStep);
}

bool operator==(const ClasterKey& left, const ClasterKey& right) {
	return left.type == right.type && left.x == right.x && left.y == right.y && left.z == right.z;
}

bool operator!=(const ClasterKey& left, const ClasterKey& right) {
	return !(left == right);
}

bool operator<(const ClasterKey& left, const ClasterKey& right) {
	return left.type < right.type ||
	(left.type == right.type && left.x < right.x) ||
	(left.type == right.type && left.x == right.x && left.y < right.y) ||
	(left.type == right.type && left.x == right.x && left.y == right.y && left.z < right.z);
}

bool operator>(const ClasterKey& left, const ClasterKey& right) {
	return left != right && !(left < right);
}

bool operator>=(const ClasterKey& left, const ClasterKey& right) {
	return !(left < right);
}

bool operator<=(const ClasterKey& left, const ClasterKey& right) {
	return !(left > right);
}


// Struct Claster

Claster::Claster(void) : count(0), traces() {}

Claster::Claster(const Claster& other) : count(other.count), traces(other.traces) {}

Claster::~Claster(void) {}

Claster& Claster::operator=(const Claster& other) {
	if (this != &other) {
		count = other.count;
		traces = other.traces;
	}
	return *this;
}


// Class PhotonMap

PhotonMap::PhotonMap(void) :
_sizeGlobal(0),
_sizeCaustic(0),
_sizeVolume(0),
totalPhotons(0),
estimate(0),
gridStep(0),
totalPow(),
type(NO)
{}

PhotonMap::PhotonMap(const PhotonMap& other) :
_sizeGlobal(0),
_sizeCaustic(0),
_sizeVolume(0),
totalPhotons(other.totalPhotons),
estimate(other.estimate),
gridStep(other.gridStep),
totalPow(other.totalPow),
type(other.type)
{}

PhotonMap::~PhotonMap(void) {
	deleteTraces();
}

PhotonMap& PhotonMap::operator=(const PhotonMap& other) {
	if (this != &other) {
		deleteTraces();
		totalPhotons = other.totalPhotons;
		estimate = other.estimate;
		gridStep = other.gridStep;
		totalPow = other.totalPow;
		type = other.type;
		for (auto claster = other.begin(), End = other.end(); claster != End; ++claster)
			for (auto trace = claster->second.traces.begin(), End = claster->second.traces.end(); trace != End;  ++trace)
				set_trace((*trace)->clone());
	}
	return *this;
}

int PhotonMap::get_size(MapType type) const {
	switch (type) {
		case GLOBAL:	return _sizeGlobal;
		case CAUSTIC:	return _sizeCaustic;
		case VOLUME:	return _sizeVolume;
		default:		break;
	}
	return (int)size();
}

void PhotonMap::swap_(PhotonMap& other) {
	swap(other);
	std::swap(_sizeGlobal, other._sizeGlobal);
	std::swap(_sizeCaustic, other._sizeCaustic);
	std::swap(_sizeVolume, other._sizeVolume);
	std::swap(totalPhotons, other.totalPhotons);
	std::swap(estimate, other.estimate);
	std::swap(gridStep, other.gridStep);
	std::swap(totalPow, other.totalPow);
	std::swap(type, other.type);
}

void PhotonMap::clear_(void) {
	clear();
	counter(RESET);
}

void PhotonMap::deleteTraces(void) {
	for (auto claster = begin(), END = end(); claster != END; ++claster) {
		for (auto trace = claster->second.traces.begin(), End = claster->second.traces.end(); trace != End;  ++trace) {
			delete *trace;
			*trace = NULL;
		}
	}
	clear_();
}

void PhotonMap::settotalPow(a_scenerys_t& lightsIdx) {
	for (auto light = lightsIdx.begin(), End = lightsIdx.end(); light != End; ++light) {
		totalPow.addition(totalPow, Power((*light)->light.light));
	}
	totalPow.product(TOTAL_PHOTONS_POWER);
}

void PhotonMap::photonRayTracing_lll(a_scenerys_t& scenerys, photonRays_t& rays) {
	for (auto ray = rays.begin(), end = rays.end(); ray != end; ++ray) {
		tracePhotonRay(scenerys, *ray);
	}
}

void PhotonMap::tracePhotonRay(a_scenerys_t& scenerys, Ray& ray) {
	if (ray.recursion <= RECURSION_DEPTH) {
		if (ray.closestScenery(scenerys, _INFINITY)) {
			Power	color(ray.scnr->get_iColor(ray));
			float	reflective = ray.scnr->reflective;
			float	refractive = ray.scnr->refractive;
			float	diffusion = ray.scnr->diffusion;
			Power	chance(ray.pow, color, reflective, refractive, diffusion);
			float	rand_ = random_double();
			if (rand_ <= chance.refl) {
				ray.getNormal();
				ray.photonReflection();
				tracePhotonRay(scenerys, ray);
			} else if (rand_ <= chance.refl + chance.refr) {
				ray.getNormal();
				if (ray.photonRefraction(chance, color, refractive, ray.scnr->matIOR, ray.scnr->matOIR)) {
					tracePhotonRay(scenerys, ray);
				}
			} else if (rand_ <= chance.refl + chance.refr + chance.diff) {
				ray.getNormal();
				ray.newPhotonTrace(type, chance, color, diffusion, ray.scnr->get_id());
				ray.randomCosineWeightedDirectionInHemisphere();
				tracePhotonRay(scenerys, ray);
			}
		}
	}
}

void PhotonMap::make(a_scenerys_t& scenerys, a_scenerys_t& lightsIdx) {
	if (type != NO) {
		photonRays_t rays;
		settotalPow(lightsIdx);
		for (auto it = lightsIdx.begin(), End = lightsIdx.end(); it != End; ++it) {
			Power pow((*it)->light.light);
			int n = pow.maxBand() / totalPow.maxBand() * totalPhotons;
			(*it)->photonEmissions(n, *this, rays);
		}
		photonRayTracing_lll(scenerys, rays);
		deleteTraces();
		for (auto ray = rays.begin(), End = rays.end(); ray != End; ++ray) {
			for (auto trace = ray->traces.begin(), end = ray->traces.end(); trace != end; ++trace) {
				set_trace(*trace);
			}
		}
		outputPhotonMapParametrs();
	}
}

void PhotonMap::lookat(const Position& eye, const LookatAux& aux, float roll) {
	if (type == NO) return;
	PhotonMap tmp(*this);
	for (auto claster = begin(), End = end(); claster != End; ++claster) {
		for (auto trace = claster->second.traces.begin(), End = claster->second.traces.end(); trace != End;  ++trace) {
			(*trace)->pos.lookat(eye, aux, roll);
			tmp.set_trace(*trace);
		}
	}
	swap_(tmp);
	tmp.clear_();
}

void PhotonMap::randomDirectionsSampling(int n, const Position& pos, const Power& pow, photonRays_t& rays, bool cosineWeighted) const {
	if (cosineWeighted) {
		LookatAux aux(pos.n);
		for (int i = 0; i < n; i++)
			rays.emplace_back(pos, pow, aux);
	} else {
		for (int i = 0; i < n; i++)
			rays.emplace_back(pos, pow);
	}
}

void PhotonMap::outputPhotonMapParametrs(void) {
	if (type == NO) return;
	std::cout << "\nPHOTON MAP" << std::endl;
	std::cout << "Emitted photons......" << totalPhotons << std::endl;
	std::cout << "Grid step............" << gridStep << std::endl;
	std::cout << "Total photon power..." << totalPow << std::endl;
	std::cout << "Assessment number...." << estimate << std::endl;
	std::cout << "Photon trasces maps:"  << std::endl;
	std::cout << "  clasters..........." << size() << std::endl;
	std::cout << "  caustic traces....." << _sizeCaustic << std::endl;
	std::cout << "  global traces......" << _sizeGlobal << std::endl;
	std::cout << "  volume traces......" << _sizeVolume << std::endl;
}

