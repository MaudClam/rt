//
//  Ray.hpp
//  rt
//
//  Created by uru on 05/07/2024.
//

#ifndef RAY_HPP
# define RAY_HPP

# include "geometry.hpp"
# include "ARGBColor.hpp"

class Ray {
protected:
	void*		addr;		// image memory pixel address
	int			bytespp;	// bytes per image pixel
	Vec2f		pos;		// relative xy-coordinate on RT canvas of width 1
	float		tan;
public:
	Vec3f		pov;		// POV (point of view)
	Vec3f		dir;		// ray direction vector (normalized)
	float		dist;		// variable to record distance from pov to object
	ARGBColor	color;		// variable to record pixel color
	int			generation;	// number of reflections, refractions, absorptions
	Ray(void);
	~Ray(void);
	Ray(void* addr, int bytespp, const Vec2f& pos, float tan = 1);
	Ray(const Ray& other);
	Ray& operator=(const Ray& other);
	void reset(float tan);
	void drawPixel(void);
};

// Non member functions

float d_calculate(int fov); // fov - horizontal field of view in degrees in range [0,180]

#endif /* RAY_HPP */
