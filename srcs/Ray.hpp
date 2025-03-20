#ifndef RAY_HPP
#define RAY_HPP

# include <forward_list>
# include "geometry.hpp"
# include "ARGBColor.hpp"
# include "Rgb.hpp"
# include "random.hpp"
# include "PhotonMap.hpp"


class	A_Scenery;
struct	Scenerys;
class	PhotonMap;

struct	Ray;
struct	HitRecord;
struct	Rays;

enum Choice {
	ABSORPTION,
	REFLECTION,
	PARTIAL_REFLECTION,
	FULL_REFLECTION,
	REFRACTION,
	DIFFUSION
};

struct HitRecord {
	Rgb		paint;	// paint for pixel
	Vec3f	pov;	// ray POV (point of view)		| photon position
	Vec3f	dir;	// normalized: ray direction	| photon incident direction
	Vec3f	norm;	// normalized: normal vector from the ray hit point
	Hit		hit;	// type of contact with an object
	A_Scenery*	scnr;	// pointer to scenery
	HitRecord(void);
	~HitRecord(void);
	HitRecord(const HitRecord& other);
	HitRecord(Ray& ray);
	HitRecord& operator=(const HitRecord& other);
};


struct Ray : public HitRecord {
	struct	Point;
	struct	Segment;
	class	Path;
	
	struct Segments : public std::forward_list<Segment> {
		Segments(void) : std::forward_list<Segment>() {}
		~Segments(void) {}
		Segments& clear_(void) {
			if (!empty())
				clear();
			return *this;
		}
	};
	
	class Path {
		union {
			struct { bool _r, _d, _v; };
		};
	public:
		Path(void) : _r(false), _d(false), _v(false) {}
		~Path(void) {}
		Path(const Path& other) : _r(other._r), _d(other._d), _v(other._d) {}
		Path& operator=(const Path& other) {
			if (this != &other) {
				_r = other._r;
				_d = other._d;
				_v = other._v;
			}
			return *this;
		}
		inline void clear(void) { _r = false; _d = false; _v = false; }
		inline void reflPhoton(void) { _r = true; }
		inline void refrPhoton(void) { _r = true; }
		inline void diffPhoton(void) { _d = true; }
		inline void volmPhoton(void) { _v = true; }
		inline void diffusion(bool d) { _d = d; }
		inline bool diffusion(void) const { return _d; }
		inline bool isGlobal(void) const { return true; }
		inline bool isCaustic(void) const { return _r; }
		inline bool isVolume(void) const { return _r || _d || _v; }
	};
	
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
	Path		path;			// ray path		| photon path
	Segment		intersections;	// segment on ray - scenery entry and exit points
	ARGBColor	light, shine, color;
	CombineType	combineType;	// type of object combination
	Segments	segments;		// container for segments handling
	Traces		traces;			//				| photon traces
	
	Ray(void);
	Ray(const Position pos, const Power& _pow);
	Ray(const Position pos, const Power& _pow, const LookatAux& aux);
	~Ray(void);
	Ray(const Ray& other);
	Ray& operator=(const Ray& other);
	Ray& operator=(const HitRecord& other);
	Ray& reset(HitRecord& rec);
	Ray& restore(const HitRecord& rec);
	Ray& set_hit(Hit hit);
	Ray& getNormal(void);
	Ray& combination(void);
	Ray& union_(Segment& segment1, Segment& segment2);
	Ray& subtraction(Segment& segment1, Segment& segment2);
	Ray& intersection(Segment& segment1, Segment& segment2);
	Ray& combine(auto& scenery, auto& end, float distance, Hit target);
	Ray& markPath(void);
	Ray& fakeAmbientLighting(HitRecord& rec, const Rgb& ambient);
	Ray& directLightings(HitRecord& rec, const Scenerys& scenerys, const Scenerys& lightsIdx);
	Ray& phMapLightings(HitRecord& rec, const PhotonMap& phMap, MapType type);
	bool end(const Scenerys& scenerys, const Lighting& background, int depth, int r);
	bool closestScenery(const Scenerys& scenerys, float maxDistance, Hit target = FRONT);
	bool isGlowing(void);
	float probability(Choice choice, bool isPhoton);
	Choice chooseDirection(HitRecord& rec, bool isPhoton);
	int	   getAttenuation(HitRecord& rec, Choice choice, float& intensity, float& shining);
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
		pov.addition(pov, norm.product(distance));
	}
	inline void movePovByNormal(const HitRecord& rec, float distance) {
		pov.addition(rec.pov, rec.norm * distance);
	}
	inline void reflect(float mattness = 0) {
		dir.reflect(norm);
		getMatt(mattness);
		movePovByNormal(EPSILON);
	}
	inline bool refract(float eta, float& schlick, float mattness = 0) {
		float cos_theta = -(dir * norm);
		if (dir.refract_(norm, cos_theta, eta)) {
			getMatt(mattness);
			schlick = schlick == -1 ? 0 : getSchlick(cos_theta, eta);
			movePovByNormal(-EPSILON);
			return true;
		}
		schlick = 1;
		return false;
	}
	inline void diffusion(void) {
		dir.randomInUnitHemisphere(norm);
		movePovByNormal(EPSILON);
	}
	inline void getMatt(float mattness) {
		if (mattness)
			dir.addition(dir, Vec3f().randomInSphere(mattness)).normalize();
	}
	inline void resetColors(void) {
		color.val = shine.val = light.val = 0;
	}
	inline void photonReflection(void) {
		movePovByNormal(EPSILON);
		dir.reflect(norm);
		path.reflPhoton();
//		recursion++;
	}
	inline bool photonRefraction(const Power& chance, const Power& color, float refractive, float matIOR, float matOIR) {
		if (dir.refract(norm, hit == INSIDE ? matIOR : matOIR)) {
			movePovByNormal(-EPSILON);
			pow.refrAdjust(chance, color, refractive);
			path.refrPhoton();
			return true;
		}
		return false;
	}
	inline void newPhotonTrace(MapType type, const Power& chance, const Power& color, float diffusion, int sceneryId) {
		switch (type) {
			case CAUSTIC: {
				if (path.isCaustic())
					traces.push_front(new PhotonTrace(CAUSTIC, pov, dir, pow, sceneryId));
				break;
			}
			case GLOBAL: {
				if (path.isCaustic())
					traces.push_front(new PhotonTrace(CAUSTIC, pov, dir, pow, sceneryId));
				if (path.isGlobal())
					traces.push_front(new PhotonTrace(GLOBAL, pov, dir, pow, sceneryId));
				break;
			}
			case VOLUME: {
				if (path.isCaustic())
					traces.push_front(new PhotonTrace(CAUSTIC, pov, dir, pow, sceneryId));
				if (path.isGlobal())
					traces.push_front(new PhotonTrace(GLOBAL, pov, dir, pow, sceneryId));
				if (path.isVolume())
					traces.push_front(new PhotonTrace(VOLUME, pov, dir, pow, sceneryId));
				break;
			}
			default:
				break;
		}
		pow.diffAdjust(chance, color, diffusion);
		path.diffPhoton();
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
			light.attenuate(scenery_iColor);
			color += light;
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


struct Rays : public std::vector<Ray> {
	Rays(void) : std::vector<Ray>() {}
	~Rays(void) {}
	Rays& clear_(int n = 0) {
		Rays tmp;
		if (n) tmp.reserve(n);
		swap(tmp);
		return *this;
	}
};


#endif /* RAY_HPP */
