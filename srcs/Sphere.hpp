//
//  Sphere.hpp
//  rt
//
//  Created by uru on 08/07/2024.
//

#ifndef SPHERE_HPP
# define SPHERE_HPP

# include "Header.h"

class	AScenery;
class	Camera;
struct	Ray;
struct	Position;

class Sphere : public AScenery {
protected:
	float _radius;
public:
	Sphere(void);
	~Sphere(void);
	Sphere(const Vec3f& center, float radius, const ARGBColor& color);
	Sphere(const Sphere& other);
	Sphere& operator=(const Sphere& other);
	bool intersection(Ray& ray, int cameraIdx, float cameraRoll = 0, Side side = FRONT) const;
	void lighting(Ray& ray, int cameraIdx, float cameraRoll = 0) const;
	void hit(Ray& ray, int cameraIdx, float cameraRoll = 0) const;
	friend std::ostream& operator<<(std::ostream& o, Sphere& sp);
	friend std::istringstream& operator>>(std::istringstream& is, Sphere& sp);
};

#endif /* SPHERE_HPP */
