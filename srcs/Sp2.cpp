
#include "Sp2.hpp"

Sp2::Sp2(void) : A_Scenery("sphere2", "sp2", false), _sphere(NULL) {}
Sp2::Sp2(const std::string& name, const std::string& nick, Sphere2* sphere) :
A_Scenery(name, nick, false), _sphere(sphere) {}
Sp2::~Sp2(void) {
	if (_sphere != NULL) {
		delete _sphere;
		_sphere = NULL;
	}
}
Sp2::Sp2(const Sp2& other) : A_Scenery(other), _sphere(other._sphere) {}

Sp2*	Sp2::clone(void) const {
	Sp2*	sphere2 = new Sp2(*this);
	if (_sphere) {
		sphere2->_sphere = _sphere->clone();
	}
	return sphere2;
}

void  Sp2::output(std::ostringstream& os) const {
	os << *this;
}

std::ostream& operator<<(std::ostream& o, const Sp2& sp) {
	std::ostringstream os;
	os << std::setw(3) << std::left << sp._nick;
	os << sp._sphere->output_geometry();
	os << "   " << sp.output_material(sp._sphere->getTextureName_if());
	o  << std::setw(52) << std::left << os.str();
	o  << "  #" << sp._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Sp2& sp) {
	sp._sphere->set_geometry(is);
	sp.set_material(is, sp._sphere->txtr);
	return is;
}


