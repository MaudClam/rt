#ifndef RAY_HPP
#define RAY_HPP

# include <forward_list>
# include "geometry.hpp"
# include "ARGBColor.hpp"
# include "random.hpp"
# include "PhotonMap.hpp"


class	A_Scenery;
typedef	std::vector<A_Scenery*>					a_scenerys_t;
typedef	a_scenerys_t::iterator					a_scenerys_it;


class PhotonPath {
	bool	r;	// passed reflection or refraction
	bool	d;	// passed diffusion
	bool	v;	// passed volume diffusion
public:
	PhotonPath(void);
	~PhotonPath(void);
	PhotonPath(const PhotonPath& other);
	PhotonPath& operator=(const PhotonPath& other);
	inline void set_reflection(void) { r = true; }
	inline void set_refraction(void) { r = true; }
	inline void set_diffusion(void) { d = true; }
	inline void set_volume(void) { v = true; }
	inline bool is_global(void) { return true; }
	inline bool is_caustic(void) { return r; }
	inline bool is_volume(void) { return r || d || v; }
};

struct Ray;

struct HitRecord {
	Vec3f		pov;	// ray POV (point of view)		| photon position
	Vec3f		dir;	// normalized: ray direction	| photon incident direction
	Vec3f		norm;	// normalized: normal vector from the ray hit point
	Hit			hit;	// type of contact with an object
	A_Scenery*	scnr;	// pointer to scenery
	HitRecord(void);
	~HitRecord(void);
	HitRecord(const HitRecord& other);
	HitRecord(const Ray& ray);
	HitRecord& operator=(const HitRecord& other);
};


struct ColorSafe {
	int	shine;
	int	color;
	ColorSafe(Ray& ray);
	~ColorSafe(void);
	ColorSafe(const ColorSafe& other);
	ColorSafe& operator=(const ColorSafe& other);
};


struct Ray : public HitRecord {
	struct	Point;
	struct	Segment;
	typedef	std::forward_list<Segment>	segments_t;
	struct Point {
		float		d;
		bool		inside;
		A_Scenery*	s;
		Point(void) : d(0), inside(false), s(NULL) {}
		Point(float d, bool inside, A_Scenery* s) : d(d), inside(inside), s(s) {}
		~Point(void) {}
		Point(const Point& other) : d(other.d), inside(other.inside), s(other.s) {}
		Point& operator=(const Point& other) {
			if (this != &other) {
				d = other.d;
				inside = other.inside;
				s = other.s;
			}
			return *this;
		}
		Point& set(float _d, bool _inside, A_Scenery* _s) {
			d = _d;
			inside = _inside;
			s = _s;
			return *this;
		}
		inline void swap(Point& other) {
			std::swap(d, other.d);
			std::swap(inside, other.inside);
			std::swap(s, other.s);
		}
	};
	struct Segment {
		Point a;
		Point b;
		bool  removed;
		bool  combine;
		Segment(void) : a(), b() {}
		Segment(const Point& a, const Point& b) : a(a), b(b), removed(false), combine(false) {}
		Segment(float ad, bool ai, A_Scenery* as, float bd, bool bi, A_Scenery* bs, bool r, bool c) :
		a(ad,ai,as), b(bd,bi,bs), removed(r), combine(c) {}
		Segment(const Segment& other) : a(other.a), b(other.b), removed(other.removed), combine(other.combine) {}
		~Segment(void) {}
		Segment& operator=(const Segment& other) {
			if (this != &other) {
				a = other.a;
				b = other.b;
				removed = other.removed;
				combine = other.combine;
			}
			return *this;
		}
		inline void activate(A_Scenery* scenery) {
			if (scenery) {
				a.inside = false;
				b.inside = a.d == b.d ? false : true;
			}
			a.s = scenery;
			b.s = scenery;
			removed = false;
			combine = true;
		}
		inline bool empty(void) { return a.s == NULL; }
		inline void swap(Segment& other) {
			a.swap(other.a);
			b.swap(other.b);
			std::swap(removed, other.removed);
			std::swap(combine, other.combine);
		}
	};
	int			recursion;		// current recursion number
	float		dist;			// distance from POV to object hit point
	Power		pow;			//				| photon power
	PhotonPath	path;			//				| photon path
	Segment		intersections;	// segment on ray - object entry and exit points
	ARGBColor	light;			// variable for light sources
	ARGBColor	shine;			// variable for shines
	ARGBColor	color;			// variable for pixel color
	CombineType	combineType;	// type of object combination
	segments_t	segments;		// container for segments handling
	traces_t	traces;
	Ray(void);
	Ray(const Position pos, const Power& _pow);
	Ray(const Position pos, const Power& _pow, const LookatAux& aux);
	~Ray(void);
	Ray(const Ray& other);
	Ray& operator=(const Ray& other);
	Ray& operator=(const HitRecord& record);
	Ray& operator=(const ColorSafe& colorSafe);
	Ray& restore(const HitRecord& record);
	Ray& restore(const ColorSafe& colorSafe);
	Ray& restore(const HitRecord& record, const ColorSafe& colorSafe);
	Ray& set_hit(Hit hit);
	Ray& getNormal(void);
	Ray& combination(void);
	Ray& union_(Segment& segment1, Segment& segment2);
	Ray& subtraction(Segment& segment1, Segment& segment2);
	Ray& intersection(Segment& segment1, Segment& segment2);
	bool closestScenery(a_scenerys_t& scenerys, float maxDistance, Hit target = FRONT);
	Ray& combine(a_scenerys_it& scenery, a_scenerys_it& end, float distance, Hit target);	
	Ray& ambientLiting(const HitRecord& record, const ARGBColor& ambient);
	Ray& directLitings(const HitRecord& record, a_scenerys_t& scenerys, a_scenerys_t& lightsIdx);
	inline A_Scenery* getCombine(Point& nearest) {
		auto segment = segments.before_begin(), segmentNext = segments.begin();
		for (; segment != segments.end(); ++segment) {
			segmentNext = segment; segmentNext++;
			while (segmentNext != segments.end() && segmentNext->removed) {
				segmentNext = segments.erase_after(segment);
			}
			if (segmentNext != segments.end() && segmentNext->combine) {
				if (nearest.d > segmentNext->a.d && segmentNext->a.d >= 0) {
					nearest = segmentNext->a;
				}
				if (nearest.d > segmentNext->b.d && segmentNext->b.d >= 0) {
					nearest = segmentNext->b;
				}
				segmentNext->combine = false;
			}
		}
		if (nearest.s) {
			dist = nearest.d;
			hit = nearest.inside ? INSIDE : OUTSIDE;
		}
		return nearest.s;
	}
	inline void emplace(const Segment& segment, bool _combine) {
		emplace(segment.a, segment.b, _combine);
	}
	inline void emplace(const Point& a, const Point& b, bool _combine) {
		segments.emplace_front(a.d, a.inside, a.s, b.d, b.inside, b.s, false, _combine);
	}
	inline void movePovByDirToDist(void) {
		pov.addition(pov, dir * dist);
	}
	inline void movePovByNormal(float distance) {
		pov.addition(pov, norm * distance);
	}
	inline void collectLight(int scenery_iColor, float k = 1) {
		color.addition(color, (light *= scenery_iColor) * k);
	}
	inline void collectLight(int scenery_iColor, const ARGBColor& _light, float k = 1) {
		light = _light;
		collectLight(scenery_iColor, k);
	}
	inline void collectShine(const Vec3f& dirFromCam, int specular) {
		if (specular > 1) {
			float k = dir.reflect(norm) * dirFromCam;
			if (k > 0.) {
				k = std::pow(k, specular);
				shine.addition(shine, light * k);
			}
		}
	}
	inline void collectReflectiveLight(const ColorSafe& colorSafe, float factor) {
		float previous = 1. - factor;
		light.val = colorSafe.color;
		color.addition(color.product(factor), light.product(previous));
		light.val = colorSafe.shine;
		shine.addition(shine.product(factor), light);
	}
	inline void collectRefractiveLight(int scenery_iColor, const ColorSafe& colorSafe, float factor) {
		float previous = 1. - factor;
		light.val = colorSafe.color;
		color.addition(color.iProduct(scenery_iColor).product(factor), light.product(previous));
		light.val = colorSafe.shine;
		shine.addition(shine.iProduct(scenery_iColor).product(factor), light);
	}
	inline void photonReflection(void) {
		movePovByNormal(EPSILON);
		dir.reflect(norm);
		path.set_reflection();
		recursion++;
	}
	inline bool photonRefraction(const Power& chance, const Power& color, float refractive, float matIOR, float matOIR) {
		if (dir.refract(norm, hit == INSIDE ? matIOR : matOIR)) {
			movePovByNormal(-EPSILON);
			pow.refrAdjust(chance, color, refractive);
			path.set_refraction();
			return true;
		}
		return false;
	}
	inline void newPhotonTrace(MapType type, const Power& chance, const Power& color, float diffusion, int sceneryId) {
		switch (type) {
			case CAUSTIC: {
				if (path.is_caustic())
					traces.push_front(new PhotonTrace(CAUSTIC, pov, dir, pow, sceneryId));
				break;
			}
			case GLOBAL: {
				if (path.is_caustic())
					traces.push_front(new PhotonTrace(CAUSTIC, pov, dir, pow, sceneryId));
				if (path.is_global())
					traces.push_front(new PhotonTrace(GLOBAL, pov, dir, pow, sceneryId));
				break;
			}
			case VOLUME: {
				if (path.is_caustic())
					traces.push_front(new PhotonTrace(CAUSTIC, pov, dir, pow, sceneryId));
				if (path.is_global())
					traces.push_front(new PhotonTrace(GLOBAL, pov, dir, pow, sceneryId));
				if (path.is_volume())
					traces.push_front(new PhotonTrace(VOLUME, pov, dir, pow, sceneryId));
				break;
			}
			default:
				break;
		}
		pow.diffAdjust(chance, color, diffusion);
		path.set_diffusion();
		movePovByNormal(EPSILON);
		recursion++;
	}
	inline void phMapLightings(float sqRadius, int estimate, int sceneryId, int scenery_iColor) {
		struct Trace {
			float k;
			PhotonTrace* trace;
			Trace(float _k, PhotonTrace* _trace) : k(_k), trace(_trace) {}
			Trace(void) {}
		};
		std::map<float, Trace> tMap;
		float sqr = 0., k = 0.;
		for (auto trace = traces.begin(), end = traces.end(); trace != end; ++trace) {
			if ((*trace)->sceneryId == sceneryId &&
				(sqr = (pov - (*trace)->pos.p).sqnorm()) <= sqRadius &&
				(k = (*trace)->pos.n * norm * -1) > 0) {
				tMap.emplace(sqr, Trace(k, *trace));
			}
		}
		if (tMap.size() > estimate * 0.1) {
			int n = 0, ns = 0;
			Power lighting, shining;
			auto last = tMap.begin();
			for (auto it = tMap.begin(), end = tMap.end(); it != end && n < estimate; ++it) {
				lighting.addition(lighting, it->second.trace->pow * it->second.k);
				last = it;
				n++;
			}
			lighting.product((float)n / (M_PI * last->first)).getARGBColor(light);
			collectLight(scenery_iColor);
			if (ns > estimate * 0.1) {
				shining.getARGBColor(light);
				shine.addition(shine, light);
			}

		}
	}
	inline void randomUniformDirectionInHemisphere(const Vec3f& normal) {
		float phi = random_double() * M_2PI;
		float theta = random_double() * M_PI;
		dir.sphericalDirection2cartesian(phi, theta);
		if (!dir.isNull() && dir * normal < 0) {// if normal(0,0,0) then the full sphere direction will be generated
			dir.product(-1);
		}
	}
	inline void randomCosineWeightedDirectionInHemisphere(const LookatAux& aux, float width = 1.) {
		float phi = random_double() * M_2PI;
		float theta = std::acos(std::sqrt(random_double())) * width;
		dir.sphericalDirection2cartesian(phi, theta).lookatDir(aux);
	}
	inline void randomCosineWeightedDirectionInHemisphere(float width = 1. ) {
		randomCosineWeightedDirectionInHemisphere(LookatAux(norm), width);
	}
	
	
};

bool operator<(const Ray::Segment& left, const Ray::Segment& right);


#endif /* RAY_HPP */
