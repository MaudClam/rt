//
//  PhotonMap.cpp
//  rt
//
//  Created by uru on 12/10/2024.
//

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


// Class PhotonMap

PhotonMap::PhotonMap(void) :
_sizeGlobal(0),
_sizeCaustic(0),
_sizeVolume(0),
_gridStep(PHOTON_MAP_GRID_STEP)
{}

PhotonMap::PhotonMap(const PhotonMap& other) :
_sizeGlobal(0),
_sizeCaustic(0),
_sizeVolume(0),
_gridStep(other._gridStep)
{ *this = other; }

PhotonMap::~PhotonMap(void) {
	deleteTraces();
}

PhotonMap& PhotonMap::operator=(const PhotonMap& other) {
	if (this != &other) {
		deleteTraces();
		for (auto claster = other.begin(), End = other.end(); claster != End; ++claster)
			for (auto trace = claster->second.begin(), End = claster->second.end(); trace != End;  ++trace)
				set_trace((*trace)->clone());
	}
	return *this;
}

void PhotonMap::swap_(PhotonMap& other) {
	swap(other);
	std::swap(_sizeGlobal, other._sizeGlobal);
	std::swap(_sizeCaustic, other._sizeCaustic);
	std::swap(_sizeVolume, other._sizeVolume);
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

void PhotonMap::lookat(const Position& eye, const LookatAux& aux, float roll) {
	PhotonMap tmp;
	for (auto claster = begin(), End = end(); claster != End; ++claster) {
		for (auto trace = claster->second.begin(), End = claster->second.end(); trace != End;  ++trace) {
			(*trace)->pos.lookat(eye, aux, roll);
			tmp.set_trace(*trace);
		}
	}
	swap_(tmp);
	tmp.clear();
	tmp.counter(RESET);
}

void PhotonMap::deleteTraces(void) {
	for (auto claster = begin(), END = end(); claster != END; ++claster) {
		for (auto trace = claster->second.begin(), End = claster->second.end(); trace != End;  ++trace) {
			delete *trace;
			*trace = NULL;
		}
	}
	clear();
	counter(RESET);
}

void PhotonMap::randomSampleHemisphere(rand_gen_t& gen, int n, const Position& pos, const Power& pow, photonRays_t& rays, bool is_cosineDistr) const {
	int		i = 0;
	Vec3f	dir;
	while (i < n) {
		float phi = 0., theta = 0.;
		getHemisphereRandomPhiTheta(gen, pos.n, phi, theta, is_cosineDistr);
		dir.sphericalDirection2cartesian(phi, theta);
		rays.emplace_back(pos.p, dir, pow);
		i++;
	}
}

