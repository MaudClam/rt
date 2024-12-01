#include "Sphere.hpp"


Sphere::Sphere(void) : _radius(0), _sqrRadius(0), _k(), _c(0) {
	_name = "sphere";
	_nick = "sp";
	_isLight = false;
}

Sphere::~Sphere(void) {}

Sphere::Sphere(const Vec3f& center, float radius, const ARGBColor& color) :
_radius(radius), _sqrRadius(radius * radius) {
	_name = "sphere";
	_nick = "sp";
	_isLight = false;
	_pos.p = center;
	_color = color;
}

Sphere::Sphere(const Sphere& other) :
_radius(other._radius),
_sqrRadius(_radius * _radius),
_k(other._k),
_c(other._c)
{
	_id = other._id;
	_name = other._name;
	_nick = other._nick;
	_isLight = other._isLight;
	_pos = other._pos;
	_color = other._color;
	combineType = other.combineType;
	light = other.light;
	specular = other.specular;
	reflective = other.reflective;
	refractive = other.refractive;
	diffusion = other.diffusion;
	matIOR = other.matIOR;
	matOIR = other.matOIR;
}

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
	os << std::setw(2) << std::left << sp._nick;
	os << " " << sp._pos.p;
	os << " " << std::setw(5) << std::right << sp._radius * 2;
	os << "   " << sp.get_color().rrggbb();
	os << " " << std::setw(4) << std::right << sp.specular;
	os << " " << std::setw(4) << std::right << sp.reflective;
	os << " " << std::setw(4) << std::right << sp.refractive;
	os << " " << std::setw(4) << std::right << sp.matIOR;
	o  << std::setw(54) << std::left << os.str();
	o  << " #" << sp._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Sphere& sp) {
	is >> sp._pos.p >> sp._radius;
	is >> sp._color >> sp.specular >> sp.reflective >> sp.refractive >> sp.matIOR;
	sp._radius /= 2;
	sp._sqrRadius = sp._radius * sp._radius;
	sp.specular = i2limits(sp.specular, -1, 1000);
	sp.reflective = f2limits(sp.reflective, 0., 1.);
	sp.refractive = f2limits(sp.refractive, 0., 1. - sp.reflective);
	sp.diffusion = f2limits(1. -  sp.reflective - sp.refractive, 0., 1.);
	sp.matIOR = f2limits(sp.matIOR, 0.1, 10.);
	sp.matOIR = 1. / sp.matIOR;
	return is;
}
