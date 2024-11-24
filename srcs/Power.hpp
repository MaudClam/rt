#ifndef POWER_HPP
# define POWER_HPP

# include "ARGBColor.hpp"
# include "geometry.hpp"


struct Power {
	union {
		struct { float r, g, b; };
		struct { float refl, refr, diff; };
	};
	Power(void);
	Power(float _r, float _g, float _b);
	Power(const Power& p, const Power& c, float _refl, float _refr, float _diff);
	Power(const Power& other);
	Power(const ARGBColor& color);
	Power(const Vec3f& vec);
	~Power(void);
	Power& operator=(const Power& other);
	Power& operator=(const Vec3f& v);
	Power& operator=(const ARGBColor& c);
	Power& getARGBColor(ARGBColor& c);
	inline ARGBColor getARGBColor(void) {
		ARGBColor c;
		getARGBColor(c);
		return c;
	}
	Power& collectRGB(ARGBColor& c);
	Power& addition(const Power& p1, const Power& p2);
	inline Power operator+(const Power& p) { return Power(*this).addition(*this, p); }
	Power& substract(const Power& p1, const Power& p2);
	inline Power operator-(const Power& p) { return Power(*this).substract(*this, p); }
	Power& product(float f);
	inline Power operator*(float f) { return Power(*this).product(f); }
	Power& product(const Power& p1, const Power& p2);
	inline Power operator*(const Power& p) { return Power(*this).product(*this, p); }
	inline float maxBand(void) { return max_(r, g, b); }
	Power& chance(const Power& pow, const Power& color, float _refl, float _refr, float _diff);
	Power& reflAdjust(const Power& chance, float _refl);
	Power& refrAdjust(const Power& chance, const Power& color, float _refr);
	Power& diffAdjust(const Power& chance, const Power& color, float _diff);
	friend std::ostream& operator<<(std::ostream& o, const Power& p);
};


#endif /* POWER_HPP */
