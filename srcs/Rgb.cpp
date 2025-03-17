#include "Rgb.hpp"


// class Rgb

Rgb::Rgb(void) : raw{0,0,0} {}

Rgb::Rgb(const Rgb& other) : raw{0,0,0} { *this = other; }

Rgb::Rgb(int rgba) : raw{0,0,0} { *this = rgba; }

Rgb::~Rgb(void) {}

Rgb& Rgb::operator=(const Rgb& other) {
	if (this != &other) {
		for (int i = 0; i < 3; i++)
			raw[i] = other.raw[i];
	}
	return *this;
}

Rgb& Rgb::operator=(int rgba) {
	for (int i = 0; i < 3; i++)
		raw[i] = _1_255 * _getСhar(rgba, i);
	return *this;
}

Rgb& Rgb::operator+=(const Rgb& other) {
	for (int i = 0; i < 3; i++)
		raw[i] += other.raw[i];
	return *this;
}

Rgb& Rgb::operator+=(int rgba) {
	for (int i = 0; i < 3; i++)
		raw[i] += float(_1_255 * _getСhar(rgba, i));
	return *this;
}

Rgb& Rgb::operator*=(const Rgb& other) {
	for (int i = 0; i < 3; i++)
		raw[i] *= other.raw[i];
	return *this;
}

Rgb& Rgb::operator*=(int rgba) {
	for (int i = 0; i < 3; i++) {
		raw[i] *= float(_1_255 * _getСhar(rgba, i));
	}
	return *this;
}

Rgb& Rgb::operator*=(float f) {
	for (int i = 0; i < 3; i++)
		raw[i] *= f;
	return *this;
}

Rgb& Rgb::attenuate(int attenuation, float intensity) {
	if (attenuation != -1)
		*this *= attenuation;
	*this *= intensity;
	return *this;
}


// class MeanRgb
MeanRgb::MeanRgb(void) : Rgb(), _n(0) {}

MeanRgb::MeanRgb(const Rgb& other) : Rgb(), _n(0) { *this = other; }

MeanRgb::~MeanRgb(void) {}

MeanRgb& MeanRgb::operator=(const MeanRgb& other) {
	if (this != &other) {
		for (int i = 0; i < 3; i++)
			raw[i] = other.raw[i];
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
