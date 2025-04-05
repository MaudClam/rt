#include "ARGBColor.hpp"


std::string colorFormat(int bpp) {
	switch (bpp) {
		case 1: { return "GRAY_SCALE"; }
		case 3: { return "RGB"; }
		case 4: { return "ARGB"; }
	}
	return "wrong_bytespp";
}


// Struct ARGBColor

ARGBColor::ARGBColor(void) : val(0), bytespp(ARGB) {}

ARGBColor::~ARGBColor(void) {}

ARGBColor::ARGBColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A)
: b(B), g(G), r(R), a(A), bytespp(ARGB) {
	if (a != 0) {
		bytespp = ARGB;
	}
}

ARGBColor::ARGBColor(int v, int bpp) : val(v), bytespp(bpp) {
	if (bytespp == GRAY_SCALE) {
		if (val > 255) {
			val = 255;
		}
	} else {
		bytespp = (a == 0 ? RGB : ARGB);
	}
}

ARGBColor::ARGBColor(const ARGBColor& c) : val(c.val), bytespp(c.bytespp) {}

ARGBColor& ARGBColor::operator=(const ARGBColor& c) {
	if (this != &c) {
		bytespp = c.bytespp;
		val = c.val;
	}
	return *this;
}

ARGBColor& ARGBColor::addition(const ARGBColor& c1, const ARGBColor& c2) {
	for (int i = 0; i < 4; ++i) {
		raw[i] = (unsigned char)i2limits((int)c1.raw[i] + (int)c2.raw[i], 0, 255);
	}
	return *this;
}

ARGBColor& ARGBColor::substract(const ARGBColor& c1, const ARGBColor& c2) {
	for (int i = 0; i < 4; ++i) {
		raw[i] = (unsigned char)i2limits((int)c1.raw[i] - (int)c2.raw[i], 0, 255);
	}
	return *this;
}

ARGBColor& ARGBColor::product(const ARGBColor& c1, const ARGBColor& c2) {
	for (int i = 0; i < 4; ++i) {
		raw[i] = (unsigned char)((float)c1.raw[i] * (float)c2.raw[i] * _1_255);
	}
	return *this;
}

ARGBColor& ARGBColor::product(float f) {
	for (int i = 0; i < 4; ++i) {
		raw[i] = (unsigned char)f2limits((float)raw[i] * f, 0, 255);
	}
	return *this;
}


// Non member functions

std::string ARGBColor::rrggbb(void) const {
	std::ostringstream os;
	os << std::setw(3) << std::setfill('0') << (int)r << ",";
	os << std::setw(3) << std::setfill('0') << (int)g << ",";
	os << std::setw(3) << std::setfill('0') << (int)b;
	return os.str();
}

std::string ARGBColor::aarrggbb(void) const {
	std::ostringstream os;
	os << std::setw(3) << std::setfill(' ') << (int)a << ",";
	os << rrggbb();
	return os.str();
}

std::string ARGBColor::HEXaarrggbb(void) const {
	std::ostringstream s;
	s << "0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << val;
	return s.str();
}

std::string ARGBColor::HTMLrrggbb(void) const {
	std::ostringstream s;
	s << "#" << std::hex << std::uppercase;
	s << std::setw(2) << std::setfill('0') << (int)raw[2];
	s << std::setw(2) << std::setfill('0') << (int)raw[1];
	s << std::setw(2) << std::setfill('0') << (int)raw[0];
	return s.str();
}

std::ostream& operator<<(std::ostream& o, const ARGBColor& c) {
	if (c.bytespp == GRAY_SCALE) {
		o << "GRAY(" << c.val << ") ";
	} else {
		o << c.HEXaarrggbb() << " ";
		o << c.HTMLrrggbb() << " ";
		o << "RGB(" << c.rrggbb() << ") ";
		o << "ARGB(" << c.aarrggbb() << ") ";
	}
	o << colorFormat(c.bytespp);
	return o;
}

std::istringstream& operator>>(std::istringstream& is, ARGBColor& c) {
	c.val = 0;
	c.bytespp = RGB;
	std::string s;
	is >> s;
	std::istringstream iss(s);
	char trash;
	if (!s.compare(0,1,"#")) {
		iss >> trash;
		iss >> std::hex >> c.val;
	} else if (!s.compare(0,2,"0x")) {
		iss >> trash >> trash;
		iss >> std::hex >> c.val;
	} else {
		std::vector<unsigned int> v;
		while (iss) {
			unsigned int x = 0;
			iss >> x >> trash;
			x = f2limits(x, 0, 255);
			v.push_back(x);
		}
		std::vector<unsigned int>::iterator it = v.begin();
		int i = (v.size() == 4 ? 3 : 2);
		for (; it != v.end() && i >= 0 ; ++it, --i) {
			c.raw[i] = *it;
		}
		if (v.size() == 1) {
			c.b = c.g = c.r;
		}
		c.bytespp = (c.a == 0 ? RGB : ARGB);
	}
	return is;
}


// Struct Lighting

Lighting::Lighting(void) : _ratio(1), _color(0x00FFFFFF), light() {
	_ratio = f2limits(_ratio, 0, 1);
	light = _color.val;
	light *= _ratio;
}

Lighting::Lighting(float ratio, const ARGBColor& color) :
_ratio(ratio),
_color(color),
light(color.val) {
	_ratio = f2limits(_ratio, 0, 1);
	light *= _ratio;
}

Lighting::Lighting(const Lighting& other) :
_ratio(other._ratio),
_color(other._color),
light(other.light)
{}

Lighting::~Lighting(void) {}

Lighting& Lighting::operator=(const Lighting& other) {
	if (this != &other) {
		_ratio = other._ratio;
		_color = other._color;
		light = other.light;
	}
	return *this;
}


// Non member functions

std::ostream& operator<<(std::ostream& o, const Lighting& l) {
	std::ostringstream os;
	os << " " << std::setw(4) << std::right << l._ratio;
	os << " " << l._color.rrggbb();
	o << os.str();
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Lighting& l) {
	is >> l._ratio;
	is >> l._color;
	l.set_ratio(l._ratio);
	return is;
}
