#ifndef RAY_HPP
#define RAY_HPP

# include <forward_list>
# include "geometry.hpp"
# include "ARGBColor.hpp"
# include "Rgb.hpp"
# include "PhotonMap.hpp"


class	A_Scenery;
struct	Scenerys;
class	PhotonMap;

struct HitRecord;
struct PhotonTrace;
struct Traces;
struct Ray;
struct Rays;

typedef std::forward_list<PhotonTrace*> phTraces_t;

enum Choice {
	ABSORPTION,
	REFLECTION,
	PARTIAL_REFLECTION,
	FULL_REFLECTION,
	REFRACTION,
	DIFFUSION,
	DIFFUSION_IN_VOLUME
};

struct HitRecord {
	union {
		Rgb paint;		// paint for pixel
		Rgb pow;		//								| photon power
	};
	Vec3f	pov;		// ray POV (point of view)		| photon position
	Vec3f	dir;		// normalized: ray direction	| photon incident direction
	Vec3f	norm;		// normalized: normal vector from the ray hit point
	Hit		hit;		// type of contact with an object
	A_Scenery*	scnr;	// pointer to scenery
	HitRecord(void);
	~HitRecord(void);
	HitRecord(const HitRecord& other);
	HitRecord(Ray& ray, bool photon = false);
	HitRecord& operator=(const HitRecord& other);
};


struct PhotonTrace {
	MapType		type;
	Position	pos;
	Rgb			pow;
	int			scnrId;
	PhotonTrace(void);
	PhotonTrace(MapType _type, const HitRecord& rec);
	PhotonTrace(const PhotonTrace& other);
	~PhotonTrace(void);
	PhotonTrace& operator=(const PhotonTrace& other);
	inline PhotonTrace* clone(void) const { return new PhotonTrace(*this); }
};


struct Ray : public HitRecord {
	struct	Point;
	struct	Segment;
	struct	Segments;
	struct	TraceAround;
	class	Path;

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

	struct TraceAround {
		float fading;
		PhotonTrace* trace;
		TraceAround(float i, PhotonTrace* t) : fading(i), trace(t) {}
		TraceAround(void) {}
	};
	
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
		inline void mark(Choice choice) {
			switch (choice) {
				case ABSORPTION:			_d = true;	return;
				case REFLECTION:			_r = true;	return;
				case PARTIAL_REFLECTION:	_r = true;	return;
				case FULL_REFLECTION:		_r = true;	return;
				case REFRACTION:			_r = true;	return;
				case DIFFUSION:				_d = true;	return;
				case DIFFUSION_IN_VOLUME:	_v = true;	return;
			}
		}
		inline bool isDiffusion(void) const { return _d; }
		inline bool isGlobal(void) const { return true; }
		inline bool isCaustic(void) const { return _r; }
		inline bool isVolume(void) const { return _r || _d || _v; }
	};
	
	int			recursion;		// current recursion number
	float		dist;			// distance from POV to object hit point
	Path		path;			// ray path		| photon path
	Segment		intersections;	// segment on ray - scenery entry and exit points
	CombineType	combineType;	// type of object combination
	Segments	segments;		// container for segments handling
	phTraces_t	traces;			//				| photon traces
	
	Ray(void);
	Ray(const Position pos, const Rgb& _pow);
	Ray(const Position pos, const Rgb& _pow, const LookatAux& aux);
	~Ray(void);
	Ray(const Ray& other);
	Ray& operator=(const Ray& other);
	Ray& operator=(const HitRecord& other);
	Ray& reset(HitRecord& rec);
	Ray& restore(const HitRecord& rec, bool part = false);
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
	bool photonEnd(const Scenerys& scenerys, int r);
	bool closestScenery(const Scenerys& scenerys, float maxDistance, Hit target = FRONT);
	bool isGlowing(void);
	Choice chooseDirection(const HitRecord& rec, const Probability& p);
	int	   getAttenuation(HitRecord& rec, Choice choice, float& fading, float& shining);
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
	inline void diffusionInVolume(void) {
		dir.randomInUnitSphere();
		movePovByNormal(EPSILON);
	}
	inline void getMatt(float mattness) {
		if (mattness)
			dir.addition(dir, Vec3f().randomInSphere(mattness)).normalize();
	}
	inline void newPhotonTrace(MapType type, const HitRecord& rec) {
		switch (type) {
			case CAUSTIC: {
				if (path.isCaustic())
					traces.push_front(new PhotonTrace(CAUSTIC, rec));
				break;
			}
			case GLOBAL: {
				if (path.isCaustic())
					traces.push_front(new PhotonTrace(CAUSTIC, rec));
				if (path.isGlobal())
					traces.push_front(new PhotonTrace(GLOBAL, rec));
				break;
			}
			case VOLUME: {
				if (path.isCaustic())
					traces.push_front(new PhotonTrace(CAUSTIC, rec));
				if (path.isGlobal())
					traces.push_front(new PhotonTrace(GLOBAL, rec));
				if (path.isVolume())
					traces.push_front(new PhotonTrace(VOLUME, rec));
				break;
			}
			default: break;
		}
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
