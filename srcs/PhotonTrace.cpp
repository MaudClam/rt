#include "PhotonTrace.hpp"


// Struct PhotonTrace

PhotonTrace::PhotonTrace(void) : type(GLOBAL), pos(0,0,0,0,0,0), pow(0,0,0) {}

PhotonTrace::PhotonTrace(MapType _type, const Vec3f& point, const Vec3f& dir, const Power& _pow) :
type(_type),
pos(point, dir),
pow(_pow)
{}

PhotonTrace::PhotonTrace(const PhotonTrace& other) : type(other.type), pos(other.pos), pow(other.pow) {}

PhotonTrace::~PhotonTrace(void) {}

PhotonTrace& PhotonTrace::operator=(const PhotonTrace& other) {
	if (this != &other) {
		type = other.type;
		pos = other.pos;
		pow = other.pow;
	}
	return *this;
}
