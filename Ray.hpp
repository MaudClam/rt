//
//  Ray.hpp
//  rt
//
//  Created by uru on 05/07/2024.
//

#ifndef RAY_HPP
# define RAY_HPP

# include "Header.h"

struct Ray {
	Vec3f		pov;
	Vec3f		normal;
	ARGBColor	color;
	int			generation;
	Ray(void);
	~Ray(void);
	Ray(const Vec3f& p, const Vec3f& n, const ARGBColor& c, int g = 0);
	Ray(const Ray& other);
	Ray& operator=(const Ray& other);
};

#endif /* RAY_HPP */
