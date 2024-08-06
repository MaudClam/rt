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
	Vec3f		pov;	// POV (point of view)
	Vec3f		dir;	// normalized ray direction vector
	Vec3f		camDir;	// normalized camera direction vector
	Vec3f		norm;	// normalized normal vector from the ray hit point
	float		dist;	// vdistance from pov to object
	ARGBColor	light;
	ARGBColor	color;
	Ray(void);
	~Ray(void);
	Ray(const Ray& other);
	Ray& operator=(const Ray& other);
	Ray& changePov(void);
	Ray& movePovByEpsilon(void);
	Ray& reflect(const Ray& other);
	Ray& collectLight(const ARGBColor& objColor, const ARGBColor& light);
	Ray& collectLight(const ARGBColor& objColor);
	Ray& collectSpecularLight(const ARGBColor& objColor, int specular);
	Ray& collectReflectiveLight(ARGBColor& reflectColor, float reflective);

};

#endif /* RAY_HPP */
