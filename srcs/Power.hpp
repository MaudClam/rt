//
//  Power.hpp
//  rt
//
//  Created by uru on 31/10/2024.
//

#ifndef POWER_HPP
# define POWER_HPP

# include "ARGBColor.hpp"
# include "geometry.hpp"


struct Power {
	float r, g, b, max;
	Power(void);
	Power(float _r, float _g, float _b);
	Power(const Power& other);
	Power(const ARGBColor& color);
	Power(const Vec3f& vec);
	~Power(void);
	Power& operator=(const Power& other);
	Power& operator=(const Vec3f& v);
	Power& operator=(const ARGBColor& c);

	
	Power& get_Vec3f(Vec3f& v);
	inline Vec3f get_Vec3f(void) {
		Vec3f v;
		get_Vec3f(v);
		return v;
	}
	Power& get_ARGBColor(ARGBColor& c);
	inline ARGBColor get_ARGBColor(void) {
		ARGBColor c;
		get_ARGBColor(c);
		return c;
	}
	Power& addition(const Power& p1, const Power& p2);
	inline Power operator+(const Power& p) { return Power(*this).addition(*this, p); }
	Power& substract(const Power& p1, const Power& p2);
	inline Power operator-(const Power& p) { return Power(*this).substract(*this, p); }
	Power& product(float f);
	inline Power operator*(float f) { return Power(*this).product(f); }
	Power& adjust(float rA, float gA, float bA);
};

#endif /* POWER_HPP */
