#include "PhotonTrace.hpp"


// Struct PhotonTrace

PhotonTrace::PhotonTrace(void) : type(GLOBAL), pos(), pow(), sceneryId(0) {}

PhotonTrace::PhotonTrace(MapType _type, const Vec3f& point, const Vec3f& dir, const Power& _pow, int _sceneryId) :
type(_type),
pos(point, dir),
pow(_pow),
sceneryId(_sceneryId)
{}

PhotonTrace::PhotonTrace(const PhotonTrace& other) :
type(other.type),
pos(other.pos),
pow(other.pow),
sceneryId(other.sceneryId)
{}

PhotonTrace::~PhotonTrace(void) {}

PhotonTrace& PhotonTrace::operator=(const PhotonTrace& other) {
	if (this != &other) {
		type = other.type;
		pos = other.pos;
		pow = other.pow;
		sceneryId = other.sceneryId;
	}
	return *this;
}
