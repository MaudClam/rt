
#ifndef SP2_HPP
# define SP2_HPP

# include "A_Scenery.hpp"

class	Sp2 : public A_Scenery {
protected:
	Sphere2*	_sphere;
	
public:
	Sp2(void);
	Sp2(const std::string& name, const std::string& nick, Sphere2* sphere);
	~Sp2(void);
	Sp2(const Sp2& other);

	Sp2*	clone(void) const;
	inline int	getColor(const HitRecord& rec) const {
		if (_sphere->isTexture())
			return _sphere->getTextureRgba(_sphere->localHitPoint(rec.pov));
		return _color.val;
	}
	inline bool	getLight(Ray& ray) const {
		(void)ray;
		return false;
	}
	inline void 	getNormal(Ray& ray) const {
		_sphere->getNormal(ray.pov, ray.norm);
		if (ray.hit == INSIDE) {
			ray.norm.product(-1);
		}
	}
	inline void	lookat(const Position& eye, const LookatAux& aux, const Vec3f& pos, float roll) {
		(void)pos;
		_sphere->lookat(eye, aux, roll);
	}
	inline void	roll(const Vec3f& pos, float roll) {
		(void)pos;
		_sphere->roll(roll);
	}
	inline bool  intersection(Ray& ray) {
		return _sphere->intersection(ray.pov, ray.dir, ray.dist, ray.intersections.a.d, ray.intersections.b.d, ray.hit);
	}
	inline float lighting(Ray& ray) const {
		(void)ray;
		return 0;
	}
	inline bool  isGlowing(Ray& ray) const {
		(void)ray;
		return false;
	}
	inline void  photonEmissions(int num, phRays_t& rays) const {
		(void)num; (void)rays;
	}
	void  output(std::ostringstream& os) const;
	friend std::ostream& operator<<(std::ostream& o, const Sp2& s);
	friend std::istringstream& operator>>(std::istringstream& is, Sp2& sp);
};
#endif
