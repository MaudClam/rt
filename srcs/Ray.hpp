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
	Vec3f		_pov;
	Vec3f		dir;		// normalized ray direction vector
	Vec3f		_dir;
	Vec3f		dirFromCam;	// normalized camera direction vector
	Vec3f		dirToLight;	// normalized direction vector to light
	Vec3f		norm;		// normalized normal vector from the ray hit point
	Vec3f		_norm;
	float		dist;		// distance from pov to object
	ARGBColor	light;
	ARGBColor	shine;
	ARGBColor	color;
	Hit			hit;
	Ray(void);
	~Ray(void);
	Ray(const Ray& other);
	Ray& operator=(const Ray& other);
	Ray& changePov(void);
	Ray& saveHitPosition(void);
	Ray& restoreHitPosition(void);
	Ray& movePovByNormal(float distance);
	Ray& reflect(void);
	inline bool refract(float a_matIOR, Hit hit = OUTSIDE) {
		if (hit == OUTSIDE) {
			color.val = 0;
			restoreHitPosition();		}
		if (dir.refract(norm, a_matIOR)) {
			if (hit == OUTSIDE) {
				movePovByNormal(-EPSILON);
			} else {
				movePovByNormal(EPSILON);
			}
			return true;
		}
		return false;
	}
	Ray& collectLight(const ARGBColor& sceneryColor, const ARGBColor& lightSource, float k = 1);
	Ray& collectShine(const ARGBColor& sceneryColor, const ARGBColor& lightSource, int specular);
	Ray& collectReflectiveLight(int _color, int _shine, float reflective);
	Ray& collectRefractiveLight(const ARGBColor& sceneryColor, int _color, float refractive);
};

#endif /* RAY_HPP */
