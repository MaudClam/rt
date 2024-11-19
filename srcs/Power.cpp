#include "Power.hpp"


Power::Power(void) : r(0), g(0), b(0) {}

Power::Power(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {}

Power::Power(const Power& pow, const Power& color, float _refl, float _refr, float _diff) {
	chance(pow, color, _refl, _refr, _diff);
}

Power::Power(const Power& other) : r(other.r), g(other.g), b(other.b) {}

Power::Power(const ARGBColor& color) : r(0), g(0), b(0) {
	*this = color;
}

Power::Power(const Vec3f& vec) : r(0), g(0), b(0) {
	*this = vec;
}

Power::~Power(void) {}

Power& Power::operator=(const Power& other) {
	if (this != &other) {
		r = other.r;
		g = other.g;
		b = other.b;
	}
	return *this;
}

Power& Power::operator=(const Vec3f& vec) {
	r = vec.x;
	g = vec.y;
	b = vec.z;
	return *this;
}

Power& Power::operator=(const ARGBColor& color) {
	r = _1_255 * color.r;
	g = _1_255 * color.g;
	b = _1_255 * color.b;
	return *this;
}

Power& Power::get_ARGBColor(ARGBColor& c) {
	c.r = i2limits(std::round(r * 255.), 0, 255);
	c.g = i2limits(std::round(g * 255.), 0, 255);
	c.b = i2limits(std::round(b * 255.), 0, 255);
	return *this;
}

Power& Power::addition(const Power& p1, const Power& p2) {
	r = p1.r + p2.r;
	g = p1.g + p2.g;
	b = p1.b + p2.b;
	return *this;
}

Power& Power::substract(const Power& p1, const Power& p2) {
	r = p1.r - p2.r;
	g = p1.g - p2.g;
	b = p1.b - p2.b;
	return *this;
}

Power& Power::product(float f) {
	r *= f;
	g *= f;
	b *= f;
	return *this;
}

Power& Power::product(const Power& p1, const Power& p2) {
	r = p1.r * p2.r;
	g = p1.g * p2.g;
	b = p1.b * p2.b;
	return *this;
}

Power& Power::chance(const Power& pow, const Power& color, float _refl, float _refr, float _diff) {
	float maxChance = max_(color.r * pow.r, color.g * pow.g, color.b * pow.b) / max_(pow.r, pow.g, pow.b);
	refl = _refl;
	refr = maxChance * _refr;
	diff = maxChance * _diff;
	return *this;
}

Power& Power::reflAdjust(const Power& chance, float _refl) {
	r *= (_refl / chance.refl);
	g *= (_refl / chance.refl);
	b *= (_refl / chance.refl);
	return *this;
}

Power& Power::refrAdjust(const Power& chance, const Power& color, float _refr) {
	r *= (color.r * _refr / chance.refr);
	g *= (color.g * _refr / chance.refr);
	b *= (color.b * _refr / chance.refr);
	return *this;
}

Power& Power::diffAdjust(const Power& chance, const Power& color, float _diff) {
	r *= (color.r * _diff / chance.diff);
	g *= (color.g * _diff / chance.diff);
	b *= (color.b * _diff / chance.diff);
	return *this;
}

std::ostream& operator<<(std::ostream& o, const Power& p) {
	std::ostringstream os, os1;
	o << p.r << "," << p.g << "," << p.b;
	return o;
}
