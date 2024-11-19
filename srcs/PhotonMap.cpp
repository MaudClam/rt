#include "PhotonMap.hpp"


// Struct Claster

ClasterKey::ClasterKey(void) : type(ALL), x(0), y(0), z(0) {}

ClasterKey::ClasterKey(MapType _type, int _x, int _y, int _z) : type(_type), x(_x), y(_y), z(_z) {}

ClasterKey::ClasterKey(MapType type, const Vec3f point, float gridStep) : type(ALL), x(0), y(0), z(0) {
	make(type, point, gridStep);
}

ClasterKey::ClasterKey(const PhotonTrace& trace, float gridStep) : type(ALL), x(0), y(0), z(0) {
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
	x = point.x / gridStep;
	y = point.y / gridStep;
	z = point.z / gridStep;
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

PhotonMap::PhotonMap(rand_gen_t& gen) :
_gen(gen),
_sizeGlobal(0),
_sizeCaustic(0),
_sizeVolume(0),
_totalPhotons(TOTAL_PHOTONS_NUMBER),
_assessmentNumber(ASSESSMENT_PHOTONS_NUMBER),
_gridStep(PHOTON_MAP_GRID_STEP),
_totalPow()
{}

PhotonMap::PhotonMap(const PhotonMap& other) :
_gen(other._gen),
_sizeGlobal(0),
_sizeCaustic(0),
_sizeVolume(0),
_totalPhotons(0),
_assessmentNumber(ASSESSMENT_PHOTONS_NUMBER),
_gridStep(0),
_totalPow()
{ *this = other; }

PhotonMap::~PhotonMap(void) {
	deleteTraces();
}

PhotonMap& PhotonMap::operator=(const PhotonMap& other) {
	if (this != &other) {
		_totalPhotons = other._totalPhotons;
		_assessmentNumber = other._assessmentNumber;
		_gridStep = other._gridStep;
		_totalPow = other._totalPow;
		deleteTraces();
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
	std::swap(_gen, other._gen);
	std::swap(_sizeGlobal, other._sizeGlobal);
	std::swap(_sizeCaustic, other._sizeCaustic);
	std::swap(_sizeVolume, other._sizeVolume);
	std::swap(_totalPhotons, other._totalPhotons);
	std::swap(_assessmentNumber, other._assessmentNumber);
	std::swap(_gridStep, other._gridStep);
	std::swap(_totalPow, other._totalPow);
}

void PhotonMap::deleteTraces(void) {
	for (auto claster = begin(), END = end(); claster != END; ++claster) {
		for (auto trace = claster->second.traces.begin(), End = claster->second.traces.end(); trace != End;  ++trace) {
			delete *trace;
			*trace = NULL;
		}
	}
	clear();
	counter(RESET);
}

void PhotonMap::set_totalPow(a_scenerys_t& lightsIdx) {
	for (auto light = lightsIdx.begin(), End = lightsIdx.end(); light != End; ++light) {
		_totalPow.addition(_totalPow, Power((*light)->light.light));
	}
	_totalPow.product(TOTAL_PHOTONS_POWER);
}

void PhotonMap::photonRayTracing_lll(a_scenerys_t& scenerys, photonRays_t& rays) {
	rand_distr_t	distr(0.0, 1.0);
	for (auto ray = rays.begin(), end = rays.end(); ray != end; ++ray) {
		tracePhotonRay(distr, scenerys, *ray);
	}
}

void PhotonMap::tracePhotonRay(rand_distr_t& distr, a_scenerys_t& scenerys, Ray& ray) {
	if (ray.recursion <= RECURSION_DEPTH) {
		A_Scenery* scenery = ray.closestScenery(scenerys, _INFINITY);
		if (scenery) {
			Power	color(scenery->color);
			float	reflective = scenery->reflective;
			float	refractive = scenery->refractive;
			float	diffusion = f2limits(1. -  reflective - refractive, 0., 1.);
			Power	chance(ray.pow, color, reflective, refractive, diffusion);
			float	rand_ = distr(_gen);
			if (rand_ <= chance.refl) {
				scenery->giveNormal(ray);
				ray.photonReflection();
				tracePhotonRay(distr, scenerys, ray);
			} else if (rand_ <= chance.refl + chance.refr) {
				scenery->giveNormal(ray);
				if (ray.photonRefraction(chance, color, refractive, scenery->matIOR, scenery->matOIR)) {
					tracePhotonRay(distr, scenerys, ray);
				}
			} else if (rand_ <= chance.refl + chance.refr + chance.diff) {
				scenery->giveNormal(ray);
				ray.newPhotonTrace(chance, color, diffusion, scenery->get_id());
				ray.randomCosineWeightedDirectionInHemisphere(_gen);
				tracePhotonRay(distr, scenerys, ray);
			}
		}
	}
}

void PhotonMap::make(a_scenerys_t& scenerys, a_scenerys_t& lightsIdx) {
	photonRays_t rays;
	set_totalPow(lightsIdx);
	for (auto it = lightsIdx.begin(), End = lightsIdx.end(); it != End; ++it) {
		Power pow((*it)->light.light);
		int n = pow.maxBand() / _totalPow.maxBand() * _totalPhotons;
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

void PhotonMap::lookat(const Position& eye, const LookatAux& aux, float roll) {
	PhotonMap tmp(_gen);
	for (auto claster = begin(), End = end(); claster != End; ++claster) {
		for (auto trace = claster->second.traces.begin(), End = claster->second.traces.end(); trace != End;  ++trace) {
			(*trace)->pos.lookat(eye, aux, roll);
			tmp.set_trace(*trace);
		}
	}
	swap_(tmp);
	tmp.clear();
	tmp.counter(RESET);
}

void PhotonMap::randomDirectionsSampling(int n, const Position& pos, const Power& pow, photonRays_t& rays, bool cosineWeighted) const {
	if (cosineWeighted) {
		LookatAux aux(pos.n);
		for (int i = 0; i < n; i++)
			rays.emplace_back(_gen, pos, pow, aux);
	} else {
		for (int i = 0; i < n; i++)
			rays.emplace_back(_gen, pos, pow);
	}
}

void PhotonMap::outputPhotonMapParametrs(void) {
	std::cout << "\nPHOTON MAP" << std::endl;
	std::cout << "Emitted photons......" << _totalPhotons << std::endl;
	std::cout << "Grid step............" << _gridStep << std::endl;
	std::cout << "Total photon power..." << _totalPow << std::endl;
	std::cout << "Assessment number...." << _assessmentNumber << std::endl;
	std::cout << "Photon trasces maps:"  << std::endl;
	std::cout << "  global............." << _sizeGlobal << std::endl;
	std::cout << "  caustic............" << _sizeCaustic << std::endl;
	std::cout << "  volume............." << _sizeVolume << std::endl;
}

