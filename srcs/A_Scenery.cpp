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
combineType(END),
color(),
specular(-1),
reflective(0),
refractive(0),
matIOR(1),
matOIR(1)
{}

A_Scenery::~A_Scenery(void) {}

A_Scenery::A_Scenery(const A_Scenery& other) :
_name(other._name),
_nick(other._nick),
_isLight(other._isLight),
_pos(other._pos),
combineType(other.combineType),
color(other.color),
specular(other.specular),
reflective(other.reflective),
refractive(other.refractive),
matIOR(other.matIOR),
matOIR(other.matOIR)
{}

std::string A_Scenery::get_nick(void) const { return _nick; }

bool A_Scenery::get_isLight(void) const { return _isLight; }

Position A_Scenery::get_pos(void) const { return _pos; }

void A_Scenery::set_pos(const Position& pos) { _pos = pos; }


// Non member functions

std::ostream& operator<<(std::ostream& o, const A_Scenery& s) {
	std::ostringstream os;
	s.output(os);
	o << os.str();
	return o;
}
