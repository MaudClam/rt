#include "Light.hpp"


Light::Light(void) {
	_name = "light";
	_nick = "l";
	_isLight = true;
	_type = SPOTLIGHT;
}

Light::~Light(void) {}

Light::Light(const Light& other) : _type(other._type) {
	_id = other._id;
	_name = other._name;
	_nick = other._nick;
	_isLight = other._isLight;
	_pos = other._pos;
	_color = other._color;
	combineType = other.combineType;
	light = other.light;
	glossy = other.glossy;
	reflective = other.reflective;
	refractive = other.refractive;
	diffusion = other.diffusion;
	matIOR = other.matIOR;
	matOIR = other.matOIR;
	_type = other._type;
}

Light* Light::clone(void) const {
	Light* light = new Light(*this);
	return light;
}

void Light::photonEmissions(int num, const PhotonMap& phMap, photonRays_t& rays) const {
	switch (_type) {
		case SPOTLIGHT: {
			Power pow(light.light);
			phMap.randomDirectionsSampling(num, _pos, pow.product(1. / num), rays, false);
			break;
		}
		case SUNLIGHT: {
			Position pos(_pos);
			Power    pow(light.light);
			pos.p = pos.n * _INFINITY;
			pos.n.product(-1);
			phMap.randomDirectionsSampling(num * 0.5, pos, pow.product(1. / num), rays, false);
			break;
		}
		case SUNLIGHT_LIMITED: {//FIXME
			break;
		}
		default:
			break;
	}
}

void Light::output(std::ostringstream& os) const {
	os << *this;
}


// Non member functions

std::ostream& operator<<(std::ostream& o, const Light& l) {
	std::ostringstream os;
	os << std::setw(2) << std::left << l._nick;
	os << " " << l._pos.p;
	os << "   " << l.light;
	if (!l._pos.n.isNull()) {
		os << " " << l._pos.n;
	}
	o  << std::setw(56) << std::left << os.str();
	o  << " #" << l._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Light& l) {
	switch (l._type) {
		case Light::SPOTLIGHT : {
			is >> l._pos.p >> l.light;
			break;
		}
		case Light::SUNLIGHT : {
			is >> l._pos.n >> l.light;
			l._pos.n.normalize();
			break;
		}
		case Light::SUNLIGHT_LIMITED : {
			is >> l._pos.n >> l.light >> l._pos.p;
			l._pos.n.normalize();
			break;
		}
		default:
			break;
	}
	return is;
}

