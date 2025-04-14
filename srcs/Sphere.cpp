#include "Sphere.hpp"


Sphere::Sphere(void) : A_Scenery("sphere", "sp", false), _radius(0), _sqrRadius(0), _k(), _c(0) {}

Sphere::~Sphere(void) {}

Sphere::Sphere(const Sphere& other) :
A_Scenery(other),
_radius(other._radius),
_sqrRadius(_radius * _radius),
_k(other._k),
_c(other._c)
{}

Sphere* Sphere::clone(void) const {
	Sphere* sphere = new Sphere(*this);
	return sphere;
}

void Sphere::output(std::ostringstream& os) const {
	os << *this;
}

// Non member functions

std::ostream& operator<<(std::ostream& o, const Sphere& sp) {
	std::ostringstream os;
	os << std::setw(3) << std::left << sp._nick;
	os << " " << sp._pos.p;
	os << " " << std::setw(5) << std::right << sp._radius * 2;
	os << "   " << sp.output_material();
	o  << std::setw(54) << std::left << os.str();
	o  << "  #" << sp._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Sphere& sp) {
	is >> sp._pos.p >> sp._radius;
	sp._radius /= 2;
	sp._sqrRadius = sp._radius * sp._radius;
	sp.set_material(is);
	return is;
}
