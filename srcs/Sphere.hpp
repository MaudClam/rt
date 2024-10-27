//
//  Sphere.hpp
//  rt
//
//  Created by uru on 08/07/2024.
//

#ifndef SPHERE_HPP
# define SPHERE_HPP

# include "A_Scenery.hpp"

class Sphere : public A_Scenery {
protected:
	float 	_radius;
	float 	_sqrRadius;
	Vec3f	_k;
	float	_c;
public:
	Sphere(void);
	~Sphere(void);
	Sphere(const Vec3f& center, float radius, const ARGBColor& color);
	Sphere(const Sphere& other);
	Sphere* clone(void) const;
	void lookat(const Position& eye, const LookatAux& aux, const Vec3f& pos, float roll);
	void roll(const Vec3f& pos, float shiftRoll);
	bool intersection(Ray& ray) const;
	void giveNormal(Ray& ray) const;
	float getDistanceToShaderEdge(Ray& ray, float distance, bool inside) const;
	float lighting(Ray& ray) const;
	virtual void output(std::ostringstream& os) const;
	friend std::ostream& operator<<(std::ostream& o, const Sphere& sp);
	friend std::istringstream& operator>>(std::istringstream& is, Sphere& sp);
};

#endif /* SPHERE_HPP */
