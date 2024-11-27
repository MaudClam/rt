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
	inline int	get_iColor(Ray& ray) const {
		(void)ray;
		return _color.val;
	}
	inline void	lookat(const Position& eye, const LookatAux& aux, const Vec3f& pos, float roll) {
		_pos.lookat(eye, aux, roll);
		_k.substract(pos,_pos.p);
		_c = _k * _k - _sqrRadius;
	}
	inline void	roll(const Vec3f& pos, float shiftRoll) {
		if (shiftRoll != 0) {
			_pos.roll(shiftRoll);
			_k.substract(pos,_pos.p);
			_c = _k * _k - _sqrRadius;
		}
	}
	inline bool	intersection(Ray& ray) const {
		bool result = false;
		if (ray.hit == ANY_SHADOW || ray.hit == FIRST_SHADOW) {
			ray.hit = FRONT;
			result = raySphereIntersection(ray.dirL, ray.pov, _pos.p, _sqrRadius,
										   ray.dist, ray.intersections.a.d, ray.intersections.b.d,
										   ray.hit);
		} else if (!ray.recursion) {
			result = raySphereIntersection(ray.dir, _k, _c, ray.dist,
										   ray.intersections.a.d, ray.intersections.b.d,
										   ray.hit);
		} else {
			result = raySphereIntersection(ray.dir, ray.pov, _pos.p, _sqrRadius,
										   ray.dist, ray.intersections.a.d, ray.intersections.b.d,
										   ray.hit);
		}
		return result;
	}
	inline void giveNormal(Ray& ray) const {
		if (ray.hit == INSIDE) {
			normalToRaySphereIntersect(_pos.p, ray.pov, ray.norm);
		} else {
			normalToRaySphereIntersect(ray.pov, _pos.p, ray.norm);
		}
	}
	inline void photonEmissions(int num, const PhotonMap& phMap, photonRays_t& rays) const {
		(void)num; (void)rays; (void)phMap;
	}
	inline float getDistanceToShaderEdge(Ray& ray, float distance, bool inside) const {
		if (inside) {
			return distanceToSphericalShaderEdge(_pos.p,
												 ray.pov + (ray.dirL * distance),
												 ray.dirL,
												 _radius);
		}
		return distanceToSphericalShaderEdge(ray.pov + (ray.dirL * distance),
											 _pos.p,
											 ray.dirL,
											 _radius);
	}
	inline float lighting(Ray& ray) const {
		(void)ray;
		return 0;
	}
	void output(std::ostringstream& os) const;
	friend std::ostream& operator<<(std::ostream& o, const Sphere& sp);
	friend std::istringstream& operator>>(std::istringstream& is, Sphere& sp);
};

#endif /* SPHERE_HPP */
