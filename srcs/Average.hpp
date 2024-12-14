#ifndef AVERAGE_HPP
# define AVERAGE_HPP

# include "Ray.hpp"


template <class t> class	Average3;
template <class t> class	Average3x3;
typedef Average3<int>		Average3i;
typedef Average3<float>		Average3f;
typedef Average3x3<int>		Averages3x3i;
typedef Average3x3<float>	Averages3x3f;

template <class t> class Average3 : private Vec3<t> {
	int	_n;
public:
	Average3(void) : Vec3<t>(), _n(0) {}
	Average3(const Average3<t>& other) : Vec3<t>(other.Vec3<t>), _n(other._n) {}
	~Average3(void) {}
	Average3<t>& operator=(const Average3<t>& other) {
		if (this != & other) {
			_n = other._n;
			for (int i = 0; i < 3; i++)
				this->raw[i] = other.raw[i];
		}
		return *this;
	}
	Average3<t>& add(const ARGBColor& c, float weight = 1) {
		if (weight > 0) {
			for (int i = 0; i < 3; i++)
				this->raw[i] += c.raw[i] * weight;
			_n++;
		}
		return *this;
	}
	Average3<t>& getARGBColor(ARGBColor& c) {
		for (int i = 0; i < 3; i++)
			c.raw[i] = (unsigned char)i2limits( std::round( (float)this->raw[i] / (float)_n ), 0, 255 );
		return *this; }
	inline ARGBColor getARGBColor(void) {
		ARGBColor c;
		getARGBColor(c);
		return c;
	}
};


template <class t> class Average3x3 {
	union {
		struct { Average3<t> refl, refr, diff; };
		Average3<t> raw[3];
	};
public:
	Average3x3(void) : refl(), refr(), diff() {}
	Average3x3(const Average3x3<t>& other) : refl(other.refl), refr(other.refr), diff(other.diff) {}
	~Average3x3(void) {}
	Average3x3<t>& operator=(const Average3x3<t>& other) {
		if (this != &other) {
			for (int i = 0; i < 3; i++)
				this->raw[i] = other.raw[i];
		}
		return *this;
	}
	Average3x3<t>& add(const Ray& ray, float weight = 1) {
		if (weight > 0) {
			if (ray.path.isReflection()) {
				refl.add(ray.color, weight);
			} else if (ray.path.isRefraction()) {
				refr.add(ray.color, weight);
			} else if (ray.path.isDiffusion()) {
				diff.add(ray.color, weight);
			}
		}
		return *this;
	}
	Average3x3& getARGBColor(float param, Ray& ray, float reflective, float refractive, float diffusion) {
		param *= M_PI;
		reflective *= cosinePowerFading(param, SHADOW_REFL_FADING_FACTOR);
		refractive *= cosinePowerFading(param, SHADOW_REFR_FADING_FACTOR);
		diffusion  *= cosinePowerFading(param, SHADOW_DIFF_FADING_FACTOR);
		refl.getARGBColor(ray.reflections);
		ray.reflections.product(reflective);
		refr.getARGBColor(ray.refractions);
		ray.refractions.product(refractive);
		diff.getARGBColor(ray.diffusions);
		ray.diffusions.product(diffusion);
		return *this;
	}
};




#endif /* AVERAGE_HPP */
