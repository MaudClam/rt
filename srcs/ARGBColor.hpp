#ifndef ARGBCOLOR_HPP
# define ARGBCOLOR_HPP

# include <iomanip>
# include <sstream>
# include <vector>
# include "Rgb.hpp"


enum colorFormat { GRAY_SCALE=1, RGB=3, ARGB=4 };
std::string colorFormat(int bpp);


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
	inline float get_ratio(void) const { return _ratio; }
	inline int   get_glow(void) const { return _color.val; }
	inline void  set_ratio(float ratio) {
		_ratio = f2limits(ratio, 0, 1.);
		light = _color.val;
		light *= _ratio;
	}
	inline void  setLighting_if(std::istringstream& is, bool txtr) {
		if (txtr) {
			std::string trash;
			is >> _ratio >> trash;
		} else {
			is >> _ratio;
			is >> _color;
		}
		set_ratio(_ratio);
	}
	inline void  set_color(const ARGBColor& color) {
		light = color.val;
		light *= _ratio;
	}
	inline std::string outputLiting_if(const std::string& txtr) const {
		std::ostringstream os;
		os << " " << std::setw(4) << std::right << _ratio;
		if (!txtr.empty())
			os << " " << TEXTURE_KEY << txtr;
		else
			os << " " << _color.rrggbb();
		return os.str();
	}
	friend std::ostream& operator<<(std::ostream& o, const Lighting& al);
	friend std::istringstream& operator>>(std::istringstream& is, Lighting& l);
};

#endif /* ARGBCOLOR_HPP */
