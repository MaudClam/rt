//
//  A_Scenery.cpp
//  rt
//
//  Created by uru on 09/07/2024.
//

#include "A_Scenery.hpp"


// class A_Scenery

A_Scenery::A_Scenery(void) :
_name(),
_nick(),
_isLight(false),
_pos(),
lookats(),
color()
{}

A_Scenery::~A_Scenery(void) {}

A_Scenery::A_Scenery(const A_Scenery& other) :
_name(other._name),
_nick(other._nick),
_isLight(other._isLight),
_pos(other._pos),
lookats(other.lookats),
color(other.color)
{}

std::string A_Scenery::get_nick(void) const { return _nick; }

bool A_Scenery::get_isLight(void) const { return _isLight; }

Position A_Scenery::get_pos(void) const { return _pos; }

void A_Scenery::set_pos(const Position& pos) { _pos = pos; }


// Non member functions

std::ostream& operator<<(std::ostream& o, A_Scenery& s) {
	std::ostringstream os;
	s.output(os);
	o << os.str();
	return o;
}


// class BasicCoordinate

BasicCoordinate::BasicCoordinate(const Position& pos) {
	_name = "BasicCoordinate";
	_nick = "bc";
	_isLight = false;
	_pos = pos;
}

BasicCoordinate::~BasicCoordinate(void) {}

BasicCoordinate::BasicCoordinate(const BasicCoordinate& other) { *this = other; }

BasicCoordinate& BasicCoordinate::operator=(const BasicCoordinate& other) {
	if (this != &other) {
		_pos = other._pos;
		lookats = other.lookats;
	}
	return *this;
}

bool BasicCoordinate::checkLookatsIdx(int idx) const {
	if (idx >= 0 && idx < (int)lookats.size()) {
		return true;
	}
	std::cerr << "Warning: lookats index is out of range" << std::endl;
	return false;
}

void BasicCoordinate::set_lookatCamera(const Position& eye, const LookatAux& aux) {
	set_lookatBase(eye);
	lookats.back().lookAt(eye, aux);
}

void BasicCoordinate::set_lookatBase(const Position& eye) {
	(void)eye;
	lookats.push_back(Lookat(_pos));
}

void BasicCoordinate::recalculateLookat(int cam, const Position& eye, const LookatAux& aux) {
	if (checkLookatsIdx(cam)) {
		lookats[cam].lookAt(eye, aux);
	}
}

void BasicCoordinate::recalculateLookat(int cam, float roll, const Vec3f& newPov) {
	if (checkLookatsIdx(cam)) {
		(void)newPov;
		lookats[cam].set_roll(roll);
	}
}

bool BasicCoordinate::intersection(Ray& ray, int cam, Hit rayHit) const {
	(void)ray;
	(void)cam;
	(void)rayHit;
	return false;
}

void BasicCoordinate::getNormal(Ray& ray, int cam) const {
	(void)ray;
	(void)cam;
}

bool BasicCoordinate::lighting(Ray& ray, int cam) const {
	(void)ray;
	(void)cam;
	return false;
}

void BasicCoordinate::output(std::ostringstream& os) {
	os << *this;
}


// Non member functions

std::ostream& operator<<(std::ostream& o, BasicCoordinate& bc) {
	std::ostringstream os;
	os << std::setw(2) << std::left << bc._nick;
	os << " " << bc._pos.p;
	os << " " << bc._pos.n;
	o << std::setw(36) << std::left << os.str();
	o << " #" << bc._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, BasicCoordinate& bc) {
	is >> bc._pos.p >> bc._pos.n;
	bc._pos.n.normalize();
	return is;
}
