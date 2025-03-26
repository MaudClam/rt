#ifndef LIGHT_HPP
#define LIGHT_HPP

# include "A_Scenery.hpp"


class Light : public A_Scenery {
public:
	enum Type { SPOTLIGHT, SUNLIGHT, SUNLIGHT_LIMITED };
protected:
	Type     _type;
public:
	Light(void);
	~Light(void);
	Light(const Light& other);
	Light* clone(void) const;
	inline int	get_iColor(const HitRecord& rec) const {
		(void)rec;
		return light.get_albedo(); }
	inline void set_nick(const std::string& nick) { _nick = nick; }
	inline void set_name(const std::string& name) { _name = name; }
	inline void set_type(Type type) { _type = type; }
	inline void lookat(const Position& eye, const LookatAux& aux, const Vec3f& pos, float roll) {
		(void)pos;
		_pos.lookat(eye, aux, roll);
	}
	inline void roll(const Vec3f& pos, float roll) {
		(void)pos;
		_pos.roll(roll);
	}
	inline bool intersection(Ray& ray) const {
		if (_type == SUNLIGHT_LIMITED) {
			if (ray.hit == ANY_SHADOW || ray.hit == ALL_SHADOWS)
				ray.hit = FRONT;
			return rayPlaneIntersection(ray.dir, ray.pov, _pos.p, _pos.n, ray.dist, ray.intersections.a.d, ray.intersections.b.d, ray.hit);
		}
		return false;
	}
	inline void getNormal(Ray& ray) const {
		ray.norm = _pos.n;
		if (ray.dir * _pos.n > 0)
			ray.norm.product(-1);
	}
	inline float lighting(Ray& ray) const {
		float k = 0;
		switch (_type) {
			case SPOTLIGHT: {
				ray.dist = ray.dir.substract(_pos.p, ray.pov).norm();
				if (ray.dist != 0) (ray.dir.product(1. / ray.dist));// optimal normalization
				if ( (k = ray.dir * ray.norm) <= 0) {
					return 0;
				}
				break;
			}
			case SUNLIGHT: {
				if ( (k = _pos.n * ray.norm) <= 0) {
					return 0;
				}
				ray.dist = _INFINITY;
				ray.dir = _pos.n;
				break;
			}
			case SUNLIGHT_LIMITED: {
				if ( (k = _pos.n * ray.norm) <= 0) {
					return 0;
				}
				ray.dir = _pos.n;
				if (!rayPlaneIntersection(ray.dir, ray.pov, _pos.p, _pos.n, ray.dist, ray.intersections.a.d, ray.intersections.b.d, ray.hit))
					return 0;
				ray.dist -= (2 * EPSILON);
				break;
			}
			default:
				return 0;
		}
		return k;
	}
	void photonEmissions(int num, const PhotonMap& phMap, phRays_t& rays) const;
	virtual void output(std::ostringstream& os) const;
	friend std::ostream& operator<<(std::ostream& o, const Light& sp);
	friend std::istringstream& operator>>(std::istringstream& is, Light& sp);
};

#endif /* LIGHT_HPP */
