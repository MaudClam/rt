#include "Light.hpp"


Light::Light(void) : A_Scenery(), _type(SPOTLIGHT), _planar(NULL)
{
	_name = "light";
	_nick = "l";
	_isLight = true;
}

Light::Light(const std::string& name, const std::string& nick, Type type, A_Planar* planar) :
A_Scenery(name, nick, true),
_type(type),
_planar(planar)
{}

Light::~Light(void) {
	if (_planar != NULL) {
		delete _planar;
		_planar = NULL;
	}
}

Light::Light(const Light& other) : _type(other._type), _planar(other._planar) {
	_id = other._id;
	_name = other._name;
	_nick = other._nick;
	_isLight = other._isLight;
	_pos = other._pos;
	_color = other._color;
	_light = other._light;
	combineType = other.combineType;
	glossy = other.glossy;
	reflective = other.reflective;
	refractive = other.refractive;
	diffusion = other.diffusion;
	matIOR = other.matIOR;
	matOIR = other.matOIR;
}

Light* Light::clone(void) const {
	Light* light = new Light(*this);
	if (_planar)
		light->_planar = _planar->clone();
	return light;
}

void Light::photonEmissions(int num, const PhotonMap& phMap, phRays_t& rays) const {
	switch (_type) {
		case SPOTLIGHT: {
			phMap.randomDirectionsSampling(num, _pos, Rgb(_light.light) *= float(1.0f / num), rays, false);
			break;
		}
		case DIRECTLIGHT: {
			Position pos(_pos);
			pos.p = pos.n * (_INFINITY / 2);
			phMap.randomDirectionsSampling(num * 0.5, pos, Rgb(_light.light) *= float(1.0f / num), rays, true);
			break;
		}
		case SPOTLIGHT_RECTANGULAR: {//FIXME
			break;
		}
		case DIRECTLIGHT_RECTANGULAR: {//FIXME
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
	bool geometrical = true;
	std::ostringstream os;
	os << std::setw(3) << std::left << l._nick;
	switch (l._type) {
		case Light::SPOTLIGHT:               os << " " << l._pos.p << l._light; geometrical = false; break;
		case Light::SPOTLIGHT_CIRCULAR:      os << " " << l._pos.p; break;
		case Light::SPOTLIGHT_RECTANGULAR:   os << " " << l._pos.p; break;
		case Light::DIRECTLIGHT:             os << " " << l._pos.n << l._light; geometrical = false; break;
		case Light::DIRECTLIGHT_CIRCULAR:    os << " " << l._pos.n; break;
		case Light::DIRECTLIGHT_RECTANGULAR: os << " " << l._pos.n; break;
		default: break;
	}
	if (geometrical) {
		os << l._light.outputLiting_if(l._planar && l._planar->txtr ? l._planar->txtr->get_id() : "");
		os << l._planar->output_geometry();
	}
	o  << std::setw(52) << std::left << os.str();
	o  << "  #" << l._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Light& l) {
	bool spot = false;
	switch (l._type) {
		case Light::SPOTLIGHT:               is >> l._pos.p >> l._light; return is;
		case Light::SPOTLIGHT_CIRCULAR:      is >> l._pos.p; spot = true; break;
		case Light::SPOTLIGHT_RECTANGULAR:   is >> l._pos.p; spot = true; break;
		case Light::DIRECTLIGHT:             is >> l._pos.n >> l._light; l._pos.n.normalize(); return is;
		case Light::DIRECTLIGHT_CIRCULAR:    is >> l._pos.n; l._pos.n.normalize(); break;
		case Light::DIRECTLIGHT_RECTANGULAR: is >> l._pos.n; l._pos.n.normalize(); break;
		default: break;
	}
	l._light.setLighting_if(is, l._planar->txtr);
	l._planar->set_geometry(is);
	if (spot && l._pos.p.isNull())
		l._pos.p = l._planar->pos.p + l._planar->pos.n * l._planar->getMaxSize();
	if (!spot && l._pos.n.isNull())
		l._pos.n = l._planar->pos.n;
	return is;
}

