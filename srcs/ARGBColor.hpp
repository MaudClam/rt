//
//  ARGBColor.hpp
//  color
//
//  Created by uru on 02/07/2024.
//

#ifndef ARGBCOLOR_HPP
# define ARGBCOLOR_HPP

# include <iostream>
# include <iomanip>
# include <sstream>
# include <vector>


const float _1_255 = 1. / 255.;
enum colorFormat { GRAY_SCALE=1, RGB=3, ARGB=4 };
std::string colorFormat(int bpp);
int   i2limits(int num, int min, int max);
float f2limits(float num, float min, float max);
float max_(float a, float b);
float max_(float a, float b, float c);


struct ARGBColor {
	union {
		struct {
			unsigned char b, g, r, a;
		};
		unsigned char	raw[4];
		unsigned int	val;
	};
	int bytespp;
	ARGBColor(void);
	~ARGBColor(void);
	ARGBColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A = 0);
	ARGBColor(int v, int bpp = ARGB);
	ARGBColor(const ARGBColor& c);
	ARGBColor& operator=(const ARGBColor& c);
	inline ARGBColor operator+(const ARGBColor& c) const { return ARGBColor().addition(*this, c); }
	ARGBColor& addition(const ARGBColor& c1, const ARGBColor& c2);
	inline ARGBColor operator-(const ARGBColor& c) const { return ARGBColor(*this).substract(*this, c); }
	ARGBColor& substract(const ARGBColor& c1, const ARGBColor& c2);
	inline ARGBColor operator*(const ARGBColor& c) const { return ARGBColor(*this).product(*this, c); }
	ARGBColor& product(const ARGBColor& c1, const ARGBColor& c2);
	inline ARGBColor operator*(float f) const { return ARGBColor(*this).product(f); }
	ARGBColor& product(float f);
	ARGBColor& negative(void);
	std::string rrggbb(void) const;
	std::string aarrggbb(void) const;
	std::string HEXaarrggbb(void) const;
	std::string HTMLrrggbb(void) const;
	friend std::ostream& operator<<(std::ostream& o, const ARGBColor& c);
	friend std::istringstream& operator>>(std::istringstream& is, ARGBColor& c);
};


class Lighting {
	float		_ratio;
	ARGBColor	_color;
public:
	ARGBColor	light;
	Lighting(void);
	Lighting(float ratio, const ARGBColor& color = ARGBColor(0x00FFFFFF));
	~Lighting(void);
	Lighting(const Lighting& other);
	Lighting& operator=(const Lighting& other);
	float get_ratio(void) const;
	ARGBColor get_color(void) const;
	void set_ratio(float ratio);
	void set_color(const ARGBColor& color);
	void invertBrightness(void);
	friend std::ostream& operator<<(std::ostream& o, const Lighting& al);
	friend std::istringstream& operator>>(std::istringstream& is, Lighting& l);
};

#endif /* ARGBCOLOR_HPP */
