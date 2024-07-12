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
center(),
color()
{}

AScenery::~AScenery(void) {}

AScenery::AScenery(const AScenery& other) :
name(other.name),
nick(other.nick),
light(other.light),
brightness(other.brightness),
color(other.color)
{}

AScenery& AScenery::operator=(const AScenery& other) {
	if (this != &other) {
		name = other.name;
		nick = other.nick;
		light = other.light;
		brightness = other.brightness;
		color = other.color;
	}
	return *this;
}
