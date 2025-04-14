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
	inline int	 getColor(const HitRecord& rec) const {
		(void)rec;
		return _light.get_glow();
	}
	inline bool  getLight(Ray& ray) const {
		if (isPlanar()) {
			float max = ray.dist - EPSILON;
			if (_planar->planeIntersection(ray.pov, ray.dir * _planar->pos.n, ray.dist) && ray.dist < max) {
				Vec3f loc = _planar->localHitPoint(ray.pov, ray.dir, ray.dist);
				if (_planar->figureIntersection(loc, ray.set_hit(FRONT).hit)) {
					if (isTexture()) {
						ray.paint = _planar->getTextureRgba(loc);
						ray.paint *= _light.get_ratio();
					} else {
						ray.paint = get_light();
					}
					return true;
				}
			}
		} else {
			ray.paint = get_light();
			return true;
		}
		return false;
	}
	inline void  getNormal(Ray& ray) const {
		if (isPlanar())
			ray.norm = _planar->pos.p - ray.pov;
		else if (!_pos.n.isNull())
			ray.norm = _pos.n;
		else
			ray.norm = _pos.p - ray.pov;
	}
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
	inline bool  intersection(Ray& ray) {
		switch (_type) {
			case SPOTLIGHT:		return false;
			case DIRECTLIGHT:	return false;
			default: break;
		}
		return _planar->intersection(ray.pov, ray.dir, ray.dist, ray.intersections.a.d, ray.intersections.b.d, ray.set_hit(FRONT).hit);
	}
	inline float lighting(Ray& ray) const {
		ray.getDir2Light(_pos);
		float k = ray.dir * ray.norm;
		if (k > 0) {
			if (getLight(ray))
				return k;
		}
		return 0;
	}
	inline bool  isGlowing(Ray& ray) const {
		if (isPlanar()) {
			if (isTexture())
				ray.paint = _planar->getTextureRgba(_planar->localHitPoint(ray.pov, ray.dir, ray.dist));
			else
				ray.paint = _light.get_glow();
//			ray.paint *= _light.get_ratio() * LIGHTS_GLOWING_FACTOR;
		}
		return true;
	}
	void photonEmissions(int num, phRays_t& rays) const;
	void output(std::ostringstream& os) const;
	friend std::ostream& operator<<(std::ostream& o, const Light& l);

	inline void  set_type(Type type) { _type = type; }
	inline void  set_planar(A_Planar* planar) { _planar = planar; }
	inline bool  isPlanar(void) const { return _planar; }
	inline bool  isTexture(void) const { return isPlanar() && _planar->isTexture(); }
	inline bool  isMultispot(void) const { return _type == SPOTLIGHT_CIRCULAR || _type == SPOTLIGHT_RECTANGULAR; }

	inline A_Planar* get_planar(void) const { return _planar; }
	friend std::istringstream& operator>>(std::istringstream& is, Light& l);
};

#endif /* LIGHT_HPP */
