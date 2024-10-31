//
//  Power.cpp
//  rt
//
//  Created by uru on 31/10/2024.
//

#include "Power.hpp"

Power::Power(void) : r(0), g(0), b(0) {}

Power::Power(float _r, float _g, float _b) : r(_r), g(_g), b(_b), max(max_(r,g,b)) {}

Power::Power(const Power& other) : r(other.r), g(other.g), b(other.b), max(other.max) {}

Power::Power(const ARGBColor& color) : r(0), g(0), b(0), max(0) {
	*this = color;
}

Power::Power(const Vec3f& vec) : r(0), g(0), b(0), max(0) {
	*this = vec;
}

Power::~Power(void) {}

Power& Power::operator=(const Power& other) {
	if (this != &other) {
		r = other.r;
		g = other.g;
		b = other.b;
		max = other.max;
	}
	return *this;
}

Power& Power::operator=(const Vec3f& vec) {
	r = vec.x;
	g = vec.y;
	b = vec.z;
	max = max_(r, g, b);
	return *this;
}

Power& Power::operator=(const ARGBColor& color) {
	r = _1_255 * color.r;
	g = _1_255 * color.g;
	b = _1_255 * color.b;
	max = max_(r, g, b);
	return *this;
}

Power& Power::get_Vec3f(Vec3f& v) {
	v.x = r;
	v.y = g;
	v.z = b;
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
	max = max_(r, g, b);
	return *this;
}

Power& Power::substract(const Power& p1, const Power& p2) {
	r = p1.r - p2.r;
	g = p1.g - p2.g;
	b = p1.b - p2.b;
	max = max_(r, g, b);
	return *this;
}

Power& Power::product(float f) {
	r *= f;
	g *= f;
	b *= f;
	max = max_(r, g, b);
	return *this;
}

Power& Power::adjust(float rA, float gA, float bA) {
	r *= rA;
	g *= gA;
	b *= bA;
	max = max_(r, g, b);
	return *this;
}

