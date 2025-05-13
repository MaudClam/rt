#ifndef PLANAR_HPP
# define PLANAR_HPP

# include "A_Scenery.hpp"


class Planar : public A_Scenery {
public:
	enum Type { PLANE, CIRCLE, RECTANGLE, SQUARE };
protected:
	Type		_type;
	A_Planar*	_planar;
public:
	Planar(void);
	Planar(const std::string& name, const std::string& nick, Type type, A_Planar* planar);
	~Planar(void);
	Planar(const Planar& other);
	Planar* clone(void) const;
	inline int	 getColor(const HitRecord& rec) const {
		if (isTexture())
			return _planar->getTextureRgba(_planar->localHitPoint(rec.pov));
		return _color.val;
	}
	inline bool  getLight(Ray& ray) const {
		(void)ray;
		return false;
	}
	inline void  getNormal(Ray& ray) const {
		ray.norm = _planar->pos.n;
		if (ray.hit == INSIDE)
			ray.norm.product(-1);
	}
	inline void  lookat(const Position& eye, const LookatAux& aux, const Vec3f& pos, float roll) {
		(void)pos;
		_planar->lookat(eye, aux, roll);
	}
	inline void  roll(const Vec3f& pos, float roll) {
		(void)pos;
		_planar->roll(roll);
	}
	inline bool  intersection(Ray& ray) {
		return _planar->intersection(ray.pov, ray.dir, ray.dist, ray.intersections.a.d, ray.intersections.b.d, ray.hit);
	}
	inline float lighting(Ray& ray) const {
		(void)ray;
		return 0;
	}
	inline bool  isGlowing(Ray& ray) const {
		(void)ray;
		return false;
	}
	void photonEmissions(int num, phRays_t& rays) const;
	void output(std::ostringstream& os) const;
	friend std::ostream& operator<<(std::ostream& o, const Planar& l);

	inline void  set_type(Type type) { _type = type; }
	inline void  set_planar(A_Planar* planar) { _planar = planar; }
	inline bool  isPlanar(void) const { return _planar; }
	inline bool  isTexture(void) const { return isPlanar() && _planar->isTexture(); }

	inline A_Planar* get_planar(void) const { return _planar; }
	friend std::istringstream& operator>>(std::istringstream& is, Planar& l);
};


#endif /* PLANAR_HPP */
