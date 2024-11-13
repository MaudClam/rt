#ifndef RAY_HPP
#define RAY_HPP

# include <forward_list>
# include "geometry.hpp"
# include "ARGBColor.hpp"
# include "A_Scenery.hpp"
# include "PhotonTrace.hpp"


class	A_Scenery;
typedef	std::vector<A_Scenery*>	a_scenerys_t;
typedef	a_scenerys_t::iterator	a_scenerys_it;


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


struct RayBasic {
	Vec3f		pov;	// ray POV (point of view)		| photon position
	Vec3f		dir;	// normalized: ray direction	| photon incident direction
	Vec3f		dirС;	// normalized: dir from camera vector
	Vec3f		dirL;	// normalized: dir vector to light source
	Vec3f		norm;	// normalized: normal vector from the ray hit point
	float		dist;	// distance from POV to object hit point
	Hit			hit;	// type of contact with an object
	RayBasic(void);
	~RayBasic(void);
	RayBasic(const RayBasic& other);
	RayBasic& operator=(const RayBasic& other);
};


struct ColorsSafe {
	float	light;
	float	shine;
	float	color;
	ColorsSafe(void);
	~ColorsSafe(void);
	ColorsSafe(const ColorsSafe& other);
	ColorsSafe& operator=(const ColorsSafe& other);
};


struct Ray : public RayBasic {
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
	Ray(const Vec3f& _pov, const Vec3f& _dir, const Power& _pow);
	~Ray(void);
	Ray(const Ray& other);
	Ray& operator=(const Ray& other);
	Ray& operator=(const RayBasic& raySafe);
	Ray& operator=(const ColorsSafe& colorsSafe);
	Ray& getRayBasic(RayBasic& rayBasic);
	Ray& getColorsSafe(ColorsSafe& colorsSafe);
	Ray& restore(const RayBasic& raySafe);
	Ray& restore(const ColorsSafe& colorsSafe);
	Ray& set_hit(Hit hit);
	inline void emplace(const Segment& segment, bool _combine) {
		emplace(segment.a, segment.b, _combine);
	}
	inline void emplace(const Point& a, const Point& b, bool _combine) {
		segments.emplace_front(a.d, a.inside, a.s, b.d, b.inside, b.s, false, _combine);
	}
	inline void movePovByDirToDist(void) {
		pov.addition( pov, dir * dist );
	}
	inline void movePovByNormal(float distance) {
		pov.addition(pov, norm * distance);
	}
	inline void fixDirFromCam_if(void) {
		if (!recursion)
			dirС = dir;
	}
	inline void collectLight(const ARGBColor& sceneryColor, float k = 1) {
		color.addition(color, light * sceneryColor * k);
	}
	inline void collectShine(int specular, float d = 1.) {
		if (specular != -1 && d > 0.) {
			float k = dirL.get_reflect(norm) * dirС;
			if (k > 0.) {
				k = std::pow(k, specular);
				shine.addition(shine, light * (k * d));
			}
		}
	}
	inline void collectReflectiveLight(int _color, int _shine, float reflective) {
		float previous = 1. - reflective;
		light.val = _color;
		color.addition(color.product(reflective), light.product(previous));
		light.val = _shine;
		shine.addition(shine.product(reflective), light.product(previous));
	}
	inline void collectRefractiveLight(const ARGBColor& sceneryColor, int _color, float refractive) {
		float previous = 1. - refractive;
		light.val = _color;
		color.addition(color.product(color,sceneryColor).product(refractive), light.product(previous));
	}
	inline void collectShadowLight( ColorsSafe& colorsSafe, float d) {
		(void)d;
		float l = 1. - d;
		light.val = colorsSafe.light;
		light.addition(light.product(d), color.product(l));
		color.val = colorsSafe.color;
		shine.val = colorsSafe.shine;
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
	inline void newPhotonTrace(const Power& chance, const Power& color, float diffusion, A_Scenery* scenery) {
		if (path.is_global())
			traces.push_front(new PhotonTrace(GLOBAL, pov, dir, pow, scenery));
		if (path.is_caustic())
			traces.push_front(new PhotonTrace(CAUSTIC, pov, dir, pow, scenery));
		pow.diffAdjust(chance, color, diffusion);
		path.set_diffusion();
		movePovByNormal(EPSILON);
		recursion++;
	}
	inline void phMaplightings(void) {
		Power lighting;
		int n = 0;
		for (auto trace = traces.begin(), end = traces.end(); trace != end; ++trace) {
			float k = (*trace)->pos.n * norm;
			float sqr = (pov - (*trace)->pos.p).sqnorm();
			if (k < 0 && sqr < SQ_PMGS) {
				lighting.addition(lighting, (*trace)->pow * -k);
				n++;
			}
		}
		(void)n;
		lighting.product(n / (M_4PI * SQ_PMGS)).get_ARGBColor(light);
	}
	A_Scenery* closestScenery(a_scenerys_t& scenerys, float maxDistance, Hit target = FRONT);
	A_Scenery* combine(a_scenerys_it& scenery, a_scenerys_it& end, float distance, Hit target);
	A_Scenery* getCombine(Point& nearest);
	void combination(void);
	void union_(Segment& segment1, Segment& segment2);
	void subtraction(Segment& segment1, Segment& segment2);
	void intersection(Segment& segment1, Segment& segment2);
};


bool operator<(const Ray::Segment& left, const Ray::Segment& right);
std::ostream& operator<<(std::ostream& o, const Ray::Point& p);//FIXME
std::ostream& operator<<(std::ostream& o, const Ray::Segment& s);//FIXME
std::ostream& operator<<(std::ostream& o, const Ray::segments_t& s);//FIXME


#endif /* RAY_HPP */
