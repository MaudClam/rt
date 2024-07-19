//
//  Sphere.hpp
//  rt
//
//  Created by uru on 08/07/2024.
//

#ifndef SPHERE_HPP
# define SPHERE_HPP

# include "Header.h"

class AScenery;
class Camera;
class Ray;
struct Position;

class Sphere : public AScenery {
protected:
	float	radius;
public:
	Sphere(void);
	~Sphere(void);
	Sphere(const Vec3f& center, float radius, const ARGBColor& color);
	Sphere(const Sphere& other);
	Sphere& operator=(const Sphere& other);
	bool intersection(Ray& ray) const;//FIXME
	bool intersection(Ray& ray, int cameraIdx, Side side = FRONT) const;
	friend std::ostream& operator<<(std::ostream& o, Sphere& sp);
	friend std::istringstream& operator>>(std::istringstream& is, Sphere& sp);
};

#endif /* SPHERE_HPP */
