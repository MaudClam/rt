#include "A_Scenery.hpp"


// class A_Scenery

A_Scenery::A_Scenery(void) :
_id(0),
_name(),
_nick(),
_isLight(false),
_pos(),
_color(),
_light(),
combineType(END),
glossy(0),
reflective(0),
refractive(0),
diffusion(0),
matIOR(1),
matOIR(1)
{}

A_Scenery::A_Scenery(std::string name, std::string nick, bool isLight) :
_id(0),
_name(name),
_nick(nick),
_isLight(isLight),
_pos(),
_color(),
_light(),
combineType(END),
glossy(0),
reflective(0),
refractive(0),
diffusion(0),
matIOR(1),
matOIR(1)
{}

A_Scenery::~A_Scenery(void) {}

A_Scenery::A_Scenery(const A_Scenery& other) :
_id(other._id),
_name(other._name),
_nick(other._nick),
_isLight(other._isLight),
_pos(other._pos),
_color(other._color),
_light(other._light),
combineType(other.combineType),
glossy(other.glossy),
reflective(other.reflective),
refractive(other.refractive),
diffusion(other.diffusion),
matIOR(other.matIOR),
matOIR(other.matOIR)
{}


// Non member functions

std::ostream& operator<<(std::ostream& o, const A_Scenery& s) {
	std::ostringstream os;
	s.output(os);
	o << os.str();
	return o;
}
