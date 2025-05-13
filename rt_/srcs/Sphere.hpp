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
	Sphere(const Sphere& other);
	Sphere* clone(void) const;
	inline int	 getColor(const HitRecord& rec) const {
		(void)rec;
		return _color.val;
	}
	inline bool  getLight(Ray& ray) const {
		(void)ray;
		return false;
	}
	inline void  getNormal(Ray& ray) const {
		if (ray.hit == INSIDE) {
			normalToRaySphereIntersect(_pos.p, ray.pov, ray.norm);
		} else {
			normalToRaySphereIntersect(ray.pov, _pos.p, ray.norm);
		}
	}
	inline void	 lookat(const Position& eye, const LookatAux& aux, const Vec3f& pos, float roll) {
		_pos.lookat(eye, aux, roll);
		_k.substract(pos,_pos.p);
		_c = _k * _k - _sqrRadius;
	}
	inline void	 roll(const Vec3f& pos, float shiftRoll) {
		if (shiftRoll != 0) {
			_pos.roll(shiftRoll);
			_k.substract(pos,_pos.p);
			_c = _k * _k - _sqrRadius;
		}
	}
	inline bool	 intersection(Ray& ray) {
		bool result = false;
		if (ray.hit == ANY_SHADOW || ray.hit == ALL_SHADOWS) {
			ray.hit = FRONT;
			result = raySphereIntersection(ray.dir, ray.pov, _pos.p, _sqrRadius,
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
	inline void  photonEmissions(int num, phRays_t& rays) const {
		(void)num; (void)rays;
	}
	inline float lighting(Ray& ray) const {
		(void)ray;;
		return 0;
	}
	inline bool  isGlowing(Ray& ray) const {
		(void)ray;
		return false;
	}
	void output(std::ostringstream& os) const;
	friend std::ostream& operator<<(std::ostream& o, const Sphere& sp);
	friend std::istringstream& operator>>(std::istringstream& is, Sphere& sp);
};

#endif /* SPHERE_HPP */
