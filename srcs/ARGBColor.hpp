#ifndef ARGBCOLOR_HPP
# define ARGBCOLOR_HPP

# include <iomanip>
# include <sstream>
# include <vector>
# include "Rgb.hpp"


enum colorFormat { GRAY_SCALE=1, RGB=3, ARGB=4 };
std::string colorFormat(int bpp);
int   i2limits(int num, int min, int max);
float f2limits(float num, float min, float max);


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
	ARGBColor(int v, int bpp = RGB);
	ARGBColor(const ARGBColor& c);
	ARGBColor& operator=(const ARGBColor& c);
	ARGBColor& addition(const ARGBColor& c1, const ARGBColor& c2);
	ARGBColor& substract(const ARGBColor& c1, const ARGBColor& c2);
	ARGBColor& product(const ARGBColor& c1, const ARGBColor& c2);
	ARGBColor& product(float f);
	inline ARGBColor operator+(const ARGBColor& c) const { return ARGBColor().addition(*this, c); }
	inline ARGBColor operator-(const ARGBColor& c) const { return ARGBColor(*this).substract(*this, c); }
	inline ARGBColor operator*(const ARGBColor& c) const { return ARGBColor(*this).product(*this, c); }
	inline ARGBColor operator*(float f) const { return ARGBColor(*this).product(f); }
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
	Rgb			light;
	Lighting(void);
	Lighting(float ratio, const ARGBColor& color = ARGBColor(0x00FFFFFF));
	Lighting(const Lighting& other);
	~Lighting(void);
	Lighting& operator=(const Lighting& other);
	float get_ratio(void) const;
	int get_glow(void) const;
	void set_ratio(float ratio);
	void set_color(const ARGBColor& color);
	friend std::ostream& operator<<(std::ostream& o, const Lighting& al);
	friend std::istringstream& operator>>(std::istringstream& is, Lighting& l);
};

#endif /* ARGBCOLOR_HPP */
