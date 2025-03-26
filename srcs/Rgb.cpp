#include "Rgb.hpp"


// class Rgb

Rgb::Rgb(void) : b(0), g(0), r(0) {}

Rgb::Rgb(const Rgb& other) : b(0), g(0), r(0) { *this = other; }

Rgb::Rgb(int rgba) : b(0), g(0), r(0) { *this = rgba; }

Rgb::Rgb(float _r, float _g, float _b) : b(_b), g(_g), r(_r) {}

Rgb::~Rgb(void) {}

Rgb& Rgb::operator=(const Rgb& other) {
	if (this != &other) {
		for (int i = 0; i < 3; i++)
			raw[i] = other.get_band(i);
	}
	return *this;
}

Rgb& Rgb::operator=(int rgba) {
	for (int i = 0; i < 3; i++)
		raw[i] = float(_1_255 * _getСhar(rgba, i));
	return *this;
}

Rgb& Rgb::operator+=(const Rgb& other) {
	for (int i = 0; i < 3; i++)
		raw[i] += other.get_band(i);
	return *this;
}

Rgb& Rgb::operator+=(int rgba) {
	if (rgba != 0)
		for (int i = 0; i < 3; i++)
			raw[i] += float(_1_255 * _getСhar(rgba, i));
	return *this;
}

Rgb& Rgb::operator*=(const Rgb& other) {
	for (int i = 0; i < 3; i++)
		raw[i] *= other.get_band(i);
	return *this;
}

Rgb& Rgb::operator*=(int rgba) {
	for (int i = 0; i < 3; i++) {
		raw[i] *= float(_1_255 * _getСhar(rgba, i));
	}
	return *this;
}

Rgb& Rgb::operator*=(float f) {
	f = deNaN(f);
	for (int i = 0; i < 3; i++)
		raw[i] *= f;
	return *this;
}

Rgb& Rgb::attenuate(int attenuation, float fading) {
	if (attenuation != -1)
		*this *= attenuation;
	if (fading >= 0 && fading < 1)
		*this *= fading;
	return *this;
}

std::ostream& operator<<(std::ostream& o, const Rgb& p) {
	o << p.r << "," << p.g << "," << p.b;
	return o;
}


// class MeanRgb
MeanRgb::MeanRgb(void) : Rgb(), _n(0) {}

MeanRgb::MeanRgb(const Rgb& other) : Rgb(), _n(0) { *this = other; }

MeanRgb::MeanRgb(float _r, float _g, float _b) : Rgb(_r, _g, _b), _n(1) {}

MeanRgb::~MeanRgb(void) {}

MeanRgb& MeanRgb::operator=(const MeanRgb& other) {
	if (this != &other) {
		for (int i = 0; i < 3; i++)
			raw[i] = other.get_band(i);
		_n = other._n;
	}
	return *this;
}

MeanRgb& MeanRgb::operator=(int rgba) {
	for (int i = 0; i < 3; i++)
		raw[i] = _1_255 * _getСhar(rgba, i);
	_n = 1;
	return *this;
}

MeanRgb& MeanRgb::operator+=(const Rgb& rgb) {
	for (int i = 0; i < 3; i++)
		raw[i] += rgb.get_band(i);
	_n++;
	return *this;
}

MeanRgb& MeanRgb::operator+=(int rgba) {
	for (int i = 0; i < 3; i++)
		raw[i] += float(_1_255 * _getСhar(rgba, i));
	_n++;
	return *this;
}
