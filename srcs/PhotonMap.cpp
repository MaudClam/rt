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
clasters_t(),
_sizeGlobal(0),
_sizeCaustic(0),
_sizeVolume(0),
_looped(0),
totalPhotons(0),
estimate(0),
gridStep(0),
totalPow(),
type(NO)
{}

PhotonMap::PhotonMap(const PhotonMap& other) :
clasters_t(),
_sizeGlobal(0),
_sizeCaustic(0),
_sizeVolume(0),
_looped(0),
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
		_looped = other._looped;
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
	if (!empty())
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

void PhotonMap::set_trace(PhotonTrace* trace) {
	auto it_bool = try_emplace(ClasterKey().make(*trace, gridStep), Claster());
	it_bool.first->second.add_trace(trace);
	counter(trace->type);
}

void PhotonMap::photonPathsTracing_lll(Scenerys& scenerys, phRays_t& rays) {
	_looped = 0;
	for (auto ray = rays.begin(), end = rays.end(); ray != end; ++ray) {
		for(int r = 1; tracePhotonPath(scenerys, *ray, r); r++)
			if (r >= MAX_PHOTON_COLLISIONS) _looped++;
	}
}

bool PhotonMap::tracePhotonPath(Scenerys& scenerys, Ray& ray, int r) {
	if (!ray.photonEnd(scenerys, r)) {
		HitRecord rec(ray.getNormal(), true);
		Probability p;
		ray.pow *= rec.scnr->getColor(rec);
		float max = ray.pow.get_maxBand() / rec.pow.get_maxBand();
		rec.scnr->get_probability(p, max *  PHOTON_SURVIVAL);
		Choice choice = ray.chooseDirection(rec, p);
		ray.path.mark(choice);
		if (choice != ABSORPTION) {
			if (choice < REFRACTION && p.isDiffusion())
				ray.pow = rec.pow;
			ray.pow *= float(1.0 / max);
			if (choice == DIFFUSION || choice == DIFFUSION_IN_VOLUME)
				ray.newPhotonTrace(type, rec);
			return true;
		}
		if (p.isDiffusion())
			ray.newPhotonTrace(type, rec);
	}
	return false;
}

void PhotonMap::make(Scenerys& scenerys, Scenerys& lightsIdx) {
	if (type != NO) {
		phRays_t rays;
		auto Begin = lightsIdx.begin(), End = lightsIdx.end();
		for (auto lightSrc = Begin; lightSrc != End; ++lightSrc)
			totalPow += (*lightSrc)->get_light();
		totalPow *= float(TOTAL_PHOTONS_POWER);
		for (auto lightSrc = Begin; lightSrc != End; ++lightSrc) {
			int n = (*lightSrc)->get_light().get_maxBand() / totalPow.get_maxBand() * totalPhotons;
			(*lightSrc)->photonEmissions(n, rays);
		}
		photonPathsTracing_lll(scenerys, rays);
		deleteTraces();
		for (auto ray = rays.begin(), end = rays.end(); ray != end; ++ray) {
			for (auto trace = ray->traces.begin(), end = ray->traces.end(); trace != end; ++trace) {
				set_trace(*trace);
			}
		}
		outputPhotonMapParametrs();
	}
}

void PhotonMap::lookat(const Position& eye, const LookatAux& aux, float roll) {
	if (type != NO) {
		PhotonMap tmp(*this);
		for (auto claster = begin(), End = end(); claster != End; ++claster) {
			for (auto trace = claster->second.traces.begin(), end = claster->second.traces.end(); trace != end;  ++trace) {
				(*trace)->pos.lookat(eye, aux, roll);
				tmp.set_trace(*trace);
			}
		}
		swap_(tmp);
		tmp.clear_();
	}
}

void PhotonMap::outputPhotonMapParametrs(void) {
	if (type == NO) return;
	std::cout << "\nPHOTON MAP" << std::endl;
	std::cout << "Emitted photons......" << totalPhotons << std::endl;
	std::cout << "Grid step............" << gridStep << std::endl;
	std::cout << "Total photon power..." << totalPow << std::endl;
	std::cout << "Assessment number...." << estimate << std::endl;
	std::cout << "Photon traces maps:"   << std::endl;
	if (_looped)
		std::cout << "!>looped photons....." << _looped << std::endl;
	std::cout << "  clasters..........." << size() << std::endl;
	if (_sizeCaustic)
		std::cout << "  caustic traces....." << _sizeCaustic << std::endl;
	if (_sizeGlobal)
		std::cout << "  global traces......" << _sizeGlobal << std::endl;
	if (_sizeVolume)
		std::cout << "  volume traces......" << _sizeVolume << std::endl;
}

