//
//  AScenery.cpp
//  rt
//
//  Created by uru on 09/07/2024.
//

#include "AScenery.hpp"


// class AScenery

AScenery::AScenery(void) :
_name(),
_nick(),
_light(false),
_brightness(1),
_pos(),
lookats(),
color()
{}

AScenery::~AScenery(void) {}

AScenery::AScenery(const AScenery& other) :
_name(other._name),
_nick(other._nick),
_light(other._light),
_brightness(other._brightness),
_pos(other._pos),
lookats(other.lookats),
color(other.color)
{}

std::string AScenery::get_nick(void) const { return _nick; }

bool AScenery::get_light(void) const { return _light; }

float AScenery::get_brightness(void) const { return _brightness; }

Position AScenery::get_pos(void) const { return _pos; }

bool AScenery::checkLookatsIdx(int idx) const {
	if (idx >= 0 && idx < lookats.size()) {
		return true;
	}
	std::cerr << "Warning: lookats index is out of range" << std::endl;
	return false;
}

void AScenery::set_brightness(float brightness) { _brightness = brightness; }

void AScenery::set_pos(const Position& pos) { _pos = pos; }

void AScenery::set_lookatCamera(const Position& eye, const LookatAux& aux) {
	set_lookatBase();
	lookats.back().lookat(eye, aux);
}

void AScenery::set_lookatBase(void) {
	lookats.push_back(_pos);
}

void AScenery::recalculateLookat(int idx, const Position& eye, const LookatAux& aux) {
	if (checkLookatsIdx(idx)) {
		lookats[idx].lookat(eye, aux);
	}
}



// class BasicCoordinate

BasicCoordinate::BasicCoordinate(void) {
	_name = "BasicCoordinate";
	_nick = "BC";
	_light = false;
	_brightness = 0;
	_pos = Position(BASE);
}

BasicCoordinate::~BasicCoordinate(void) {}

BasicCoordinate::BasicCoordinate(const BasicCoordinate& other) { *this = other; }

BasicCoordinate& BasicCoordinate::operator=(const BasicCoordinate& other) {
	if (this != &other) {
		_brightness = other._brightness;
		_pos = other._pos;
		lookats = other.lookats;
		color = other.color;
	}
	return *this;
}

bool BasicCoordinate::intersection(Ray& ray, int cam, float roll, Side side) const {
	(void)cam;
	(void)roll;
	(void)side;
	return false;
}

void BasicCoordinate::hit(Ray& ray, int cam, float roll ) const {
	(void)cam;
	(void)roll;
}
