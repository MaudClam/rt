#ifndef LIGHT_HPP
#define LIGHT_HPP

# include "A_Scenery.hpp"


class Light : public A_Scenery {
public:
	enum Type {
		SPOTLIGHT,
		DIRECTLIGHT,
		SPOTLIGHT_CIRCULAR,
		DIRECTLIGHT_CIRCULAR,
		SPOTLIGHT_RECTANGULAR,
		DIRECTLIGHT_RECTANGULAR
	};
protected:
	Type		_type;
	A_Planar*	_planar;
public:
	Light(void);
	Light(const std::string& name, const std::string& nick, Type type, A_Planar* planar);
	~Light(void);
	Light(const Light& other);
	Light* clone(void) const;
	inline int	 get_iColor(const HitRecord& rec) const {
		(void)rec;
		return _light.get_glow(); }
	inline void  set_nick(const std::string& nick) { _nick = nick; }
	inline void  set_name(const std::string& name) { _name = name; }
	inline void  set_type(Type type) { _type = type; }
	inline void  set_planar(A_Planar* planar) { _planar = planar; }
	inline void  lookat(const Position& eye, const LookatAux& aux, const Vec3f& pos, float roll) {
		(void)pos;
		_pos.lookat(eye, aux, roll);
		if (_planar)
		_planar->lookat(eye, aux, roll);
	}
	inline void  roll(const Vec3f& pos, float roll) {
		(void)pos;
		_pos.roll(roll);
		if (_planar)
			_planar->roll(roll);
	}
	inline bool  isPlanar(void) const { return _planar; }
	inline bool  isTexture(void) const { return isPlanar() && _planar->isTexture(); }
	inline bool  intersection(Ray& ray) {
		switch (_type) {
			case SPOTLIGHT:		return false;
			case DIRECTLIGHT:	return false;
			default: break;
		}
		if (_planar->intersection(ray.pov, ray.dir, ray.dist, ray.set_hit(FRONT).hit)) {
			ray.intersections.a.d = ray.intersections.b.d = ray.dist;
			return true;
		}
		return false;
	}
	inline void  getNormal(Ray& ray) const {
		(void)ray;
	}
	inline float lighting(Ray& ray) {
		if (!_pos.n.isNull()) {
			ray.dir = _pos.n * -1;
			ray.dist = _INFINITY;
		} else {
			ray.dist = ray.dir.substract(_pos.p, ray.pov).norm();
			if (ray.dist != 0)
				ray.dir.product(1. / ray.dist);// optimal normalization
		}
		float k = ray.dir * ray.norm;
		if (k > 0) {
			if (!isPlanar()) {
				ray.paint = get_light();
				return k;
			}
			float max = ray.dist - EPSILON;
			if (_planar->planeIntersection(ray.pov, ray.dir, ray.dist) && ray.dist < max) {
				Vec3f loc = _planar->localHitPoint(ray.pov, ray.dir, ray.dist);
				if (_planar->figureIntersection(loc, ray.set_hit(FRONT).hit)) {
					if (isTexture()) {
						ray.paint = _planar->getTextureRgba(loc);
						ray.paint *= _light.get_ratio();
					} else {
						ray.paint = get_light();
					}
					return k;
				}
			}
		}
		return 0;
	}
	inline bool  isGlowing(Ray& ray) const {
		if (isTexture())
			ray.paint = _planar->getTextureRgba(_planar->localHitPoint(ray.pov, ray.dir, ray.dist));
		else
			ray.paint = _light.get_glow();
		return true;
	}

	inline A_Planar* get_planar(void) const { return _planar; }
	void photonEmissions(int num, phRays_t& rays) const;
	void output(std::ostringstream& os) const;
	friend std::ostream& operator<<(std::ostream& o, const Light& sp);
	friend std::istringstream& operator>>(std::istringstream& is, Light& sp);
};

#endif /* LIGHT_HPP */
