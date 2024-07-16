//
//  AScenery.cpp
//  rt
//
//  Created by uru on 09/07/2024.
//

#include "AScenery.hpp"

AScenery::AScenery(void) :
name(),
nick(),
light(false),
brightness(1),
pos(),
lookats(),
color()
{}

AScenery::~AScenery(void) {}

AScenery::AScenery(const AScenery& other) :
name(other.name),
nick(other.nick),
light(other.light),
brightness(other.brightness),
pos(other.pos),
lookats(other.lookats),
color(other.color)
{}

bool AScenery::checkLookatsIdx(int idx) const {
	if (idx >= 0 && idx < lookats.size()) {
		return true;
	}
	std::cerr << "Error: lookats index is out of range" << std::endl;
	return false;

}

bool AScenery::get_light(void) const {
	return light;
}

void AScenery::set_lookat(const Position& eye) {
	lookats.push_back( pos.lookat(eye) );
}

void AScenery::recalculateLookat(int idx, const Position& eye) {
	if (checkLookatsIdx(idx)) {
		lookats[idx].lookat(eye);
	}
}
