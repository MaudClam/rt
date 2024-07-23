//
//  ARGBColor.hpp
//  color
//
//  Created by uru on 02/07/2024.
//

#ifndef ARGBCOLOR_HPP
# define ARGBCOLOR_HPP

# include <iostream>
# include <sstream>
# include <iomanip>
# include <vector>

# define SPACE 0x00FFFFFF

enum colorFormat {
	GRay_SCALE=1, RGB=3, ARGB=4
};

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
	ARGBColor(int v, int bpp = ARGB);
	ARGBColor(const ARGBColor& c);
	ARGBColor& operator=(const ARGBColor& c);
	ARGBColor operator+(const ARGBColor& c) const;
	ARGBColor& addition(const ARGBColor& c1, const ARGBColor& c2);
	ARGBColor operator-(const ARGBColor& c) const;
	ARGBColor& substract(const ARGBColor& c1, const ARGBColor& c2);
	ARGBColor operator*(const ARGBColor& c) const;
	ARGBColor& product(const ARGBColor& c1, const ARGBColor& c2);
	ARGBColor operator*(float f) const;
	ARGBColor& product(float f);
	ARGBColor& negative(void);
	std::string rrggbb(void) const;
	std::string aarrggbb(void) const;
	std::string HEXaarrggbb(void) const;
	std::string HTMLrrggbb(void) const;
	friend std::ostream& operator<<(std::ostream& o, const ARGBColor& c);
	friend std::istringstream& operator>>(std::istringstream& is, ARGBColor& c);
};

	ARGBColor negative(const ARGBColor& c);

#endif /* ARGBCOLOR_HPP */
