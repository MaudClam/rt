#include "Planar.hpp"


Planar::Planar(void) : A_Scenery(), _type(PLANE), _planar(NULL)
{
	_name = "plane";
	_nick = "pl";
	_isLight = false;
}

Planar::Planar(const std::string& name, const std::string& nick, Type type, A_Planar* planar) :
A_Scenery(name, nick, false),
_type(type),
_planar(planar)
{}

Planar::~Planar(void) {
	if (_planar != NULL) {
		delete _planar;
		_planar = NULL;
	}
}

Planar::Planar(const Planar& other) :
A_Scenery(other),
_type(other._type),
_planar(other._planar) {
}

Planar* Planar::clone(void) const {
	Planar* light = new Planar(*this);
	if (_planar)
		light->_planar = _planar->clone();
	return light;
}

void Planar::photonEmissions(int num, phRays_t& rays) const {
	(void)num; (void)rays;
}

void Planar::output(std::ostringstream& os) const {
	os << *this;
}


// Non member functions

std::ostream& operator<<(std::ostream& o, const Planar& pl) {
	std::ostringstream os;
	os << std::setw(3) << std::left << pl._nick;
	os << pl._planar->output_geometry();
	os << "   " << pl.output_material(pl._planar->getTextureName_if());
	o  << std::setw(52) << std::left << os.str();
	o  << "  #" << pl._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Planar& pl) {
	pl._planar->set_geometry(is);
	pl.set_material(is, pl._planar->txtr);
	return is;
}

