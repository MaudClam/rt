//
//  Sphere.hpp
//  rt
//
//  Created by uru on 08/07/2024.
//

#ifndef SPHERE_HPP
# define SPHERE_HPP

# include "Header.h"

class	A_Scenery;
class	Camera;
struct	Ray;
struct	Position;

class Sphere : public A_Scenery {
protected:
	float _radius;
public:
	Sphere(void);
	~Sphere(void);
	Sphere(const Vec3f& center, float radius, const ARGBColor& color);
	Sphere(const Sphere& other);
	Sphere& operator=(const Sphere& other);
	bool checkLookatsIdx(int idx) const;
	void set_lookatCamera(const Position& eye, const LookatAux& aux);
	void set_lookatBase(void);
	void recalculateLookat(int idx, const Position& eye, const LookatAux& aux);
	void recalculateLookat(int idx, float roll);
	bool intersection(Ray& ray, int cameraIdx, Hit rayHit = FRONT) const;
	void getNormal(Ray& ray, int cameraIdx) const;
	bool lighting(Ray& ray, int cameraIdx) const;
	virtual void output(std::ostringstream& os);
	friend std::ostream& operator<<(std::ostream& o, Sphere& sp);
	friend std::istringstream& operator>>(std::istringstream& is, Sphere& sp);
};

#endif /* SPHERE_HPP */
