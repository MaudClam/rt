#include "PhotonTrace.hpp"


// Struct PhotonTrace

PhotonTrace::PhotonTrace(void) : type(GLOBAL), pos(), pow(), scenery(NULL) {}

PhotonTrace::PhotonTrace(MapType _type, const Vec3f& point, const Vec3f& dir, const Power& _pow, A_Scenery* _scenery) :
type(_type),
pos(point, dir),
pow(_pow),
scenery(_scenery)
{}

PhotonTrace::PhotonTrace(const PhotonTrace& other) :
type(other.type),
pos(other.pos),
pow(other.pow),
scenery(other.scenery)
{}

PhotonTrace::~PhotonTrace(void) {}

PhotonTrace& PhotonTrace::operator=(const PhotonTrace& other) {
	if (this != &other) {
		type = other.type;
		pos = other.pos;
		pow = other.pow;
		scenery = other.scenery;
	}
	return *this;
}
