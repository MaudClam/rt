//
//  Ray.hpp
//  rt
//
//  Created by uru on 01/08/2024.
//

#ifndef RAY_HPP
#define RAY_HPP

# include "geometry.hpp"
# include "ARGBColor.hpp"

struct Ray {
	int			recursion;	// recursion counter
	Vec3f		pov;		// POV (point of view)
	Vec3f		dir;		// normalized ray direction vector
	Vec3f		dirFromCam;	// normalized camera direction vector
	Vec3f		dirToLight;	// normalized direction vector to light
	Vec3f		norm;		// normalized normal vector from the ray hit point
	float		dist;		// vdistance from pov to object
	ARGBColor	light;
	ARGBColor	color;
	Ray(void);
	~Ray(void);
	Ray(const Ray& other);
	Ray& operator=(const Ray& other);
	Ray& changePov(void);
	Ray& movePovByEpsilon(void);
	Ray& reflect(void);
	Ray& collectLight(const ARGBColor& sceneryColor, const ARGBColor& lightSource, float k = 1);
	Ray& collectSpecular(const ARGBColor& sceneryColor, const ARGBColor& lightSource, int specular);
	inline int collectReflect(int _color, int reflect, float reflective) {
		color.val = _color;
		light.val = reflect;
		color.addition(color.product(1 - reflective), light.product(reflective));
		return color.val;

	}

};

#endif /* RAY_HPP */
