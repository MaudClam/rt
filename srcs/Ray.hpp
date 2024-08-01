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
	Vec3f		dir;	// ray direction vector (normalized)
	Vec3f		norm;	// normal vector at the ray hit (normalized)
	float		dist;	// vdistance from pov to object
	int			hits;	// number of ray reflections, refractions, absorptions
	ARGBColor	light;
	ARGBColor	color;
	Ray(void);
	~Ray(void);
	Ray(const Position& pos);
	Ray(const Ray& other);
	Ray& operator=(const Ray& other);
	void reflect(void);
};

#endif /* RAY_HPP */
