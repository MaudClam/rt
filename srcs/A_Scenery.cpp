#include "A_Scenery.hpp"


// class A_Scenery

A_Scenery::A_Scenery(void) :
_id(0),
_name(),
_nick(),
_isLight(false),
_pos(),
_color(),
combineType(END),
light(),
specular(-1),
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
combineType(other.combineType),
light(other.light),
specular(other.specular),
reflective(other.reflective),
refractive(other.refractive),
diffusion(other.diffusion),
matIOR(other.matIOR),
matOIR(other.matOIR)
{}

ARGBColor A_Scenery::get_color(void) const { return ARGBColor(_color); }


// Non member functions

std::ostream& operator<<(std::ostream& o, const A_Scenery& s) {
	std::ostringstream os;
	s.output(os);
	o << os.str();
	return o;
}
