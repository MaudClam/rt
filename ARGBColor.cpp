//
//  ARGBColor.cpp
//  color
//
//  Created by uru on 02/07/2024.
//

#include "ARGBColor.hpp"

std::string colorFormat(int bpp) {
	switch (bpp) {
		case 1: { return "GRAYSCALE"; }
		case 3: { return "RGB"; }
		case 4: { return "ARGB"; }
	}
	return "wrong_bytespp";
}

ARGBColor::ARGBColor(void) : val(0), bytespp(RGB) {}

ARGBColor::~ARGBColor(void) {}

ARGBColor::ARGBColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A)
: b(B), g(G), r(R), a(A), bytespp(RGB) {
	if (a != 0) {
		bytespp = ARGB;
	}
}

ARGBColor::ARGBColor(int v, int bpp) : val(v), bytespp(bpp) {
	if (bytespp == GRAYSCALE) {
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

ARGBColor ARGBColor::operator+(const ARGBColor& c) const {
	int tmp[4];
	for (int i = 0; i < 4; ++i) {
		tmp[i] = this->raw[i] + c.raw[i];
		tmp[i] = (tmp[i] > 255 ? 255 : tmp[i]);
	}
	return ARGBColor(tmp[2],tmp[1],tmp[0],tmp[3]);
}

ARGBColor ARGBColor::operator-(const ARGBColor& c) const {
	int tmp[4];
	for (int i = 0; i < 4; ++i) {
		tmp[i] = this->raw[i] - c.raw[i];
		tmp[i] = (tmp[i] < 0 ? 0 : tmp[i]);
	}
	return ARGBColor(tmp[2],tmp[1],tmp[0],tmp[3]);
}

ARGBColor ARGBColor::operator*(const ARGBColor& c) const {
	int tmp[4];
	for (int i = 0; i < 4; ++i) {
		tmp[i] = this->raw[i] * c.raw[i] * 0.003922;
	}
	return ARGBColor(tmp[2],tmp[1],tmp[0],tmp[3]);

}

ARGBColor ARGBColor::operator*(float f) const {
	int tmp[3];
	for (int i = 0; i < 3; ++i) {
		tmp[i] = this->raw[i] * f;
		tmp[i] = (tmp[i] > 255 ? 255 : tmp[i]);
	}
	return ARGBColor(tmp[2],tmp[1],tmp[0],this->raw[3]);
}

ARGBColor ARGBColor::negative(void) const {
	int tmp[3];
	for (int i = 0; i < 3; ++i) {
		tmp[i] = 255 - this->raw[i];
	}
	return ARGBColor(tmp[2],tmp[1],tmp[0],this->raw[3]);
}

std::string ARGBColor::rrggbb(void) const {
	return std::to_string( (int)r ) + "," + std::to_string( (int)g ) + "," + std::to_string( (int)b);
}

std::string ARGBColor::aarrggbb(void) const {
	return std::to_string( (int)a ) + "," + rrggbb();
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
	if (c.bytespp == GRAYSCALE) {
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
			x = (x > 255 ? 255 : x);
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
