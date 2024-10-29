//
//  Ray.hpp
//  rt
//
//  Created by uru on 01/08/2024.
//

#ifndef RAY_HPP
#define RAY_HPP

# include <forward_list>
# include "geometry.hpp"
# include "ARGBColor.hpp"
# include "A_Scenery.hpp"

class	A_Scenery;
typedef std::vector<A_Scenery*> a_scenerys_t;
typedef a_scenerys_t::iterator	a_scenerys_it;
struct	RayBasic;
struct	ColorsSafe;
struct	Ray;

struct RayBasic {
	Vec3f	pov;	// ray POV (point of view)            | photon position
	Vec3f	dir;	// normalized: ray direction          | photon incident direction
	Vec3f	pow;	// normalized: dir from camera vector | photon power
	Vec3f	dirL;	// normalized: dir vector to light source
	Vec3f	norm;	// normalized: normal vector from the ray hit point
	float	dist;	// distance from POV to object hit point
	Hit		hit;	// type of contact with an object
	RayBasic(void);
	RayBasic(const Ray& ray);
	~RayBasic(void);
	RayBasic(const RayBasic& other);
	RayBasic& operator=(const RayBasic& other);
	RayBasic& operator=(const Ray& ray);
};

struct ColorsSafe {
	float	light;
	float	shine;
	float	color;
	ColorsSafe(void);
	ColorsSafe(Ray& ray);
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
	int				recursion;		// current recursion number
	Segment			intersections;	// segment on ray - object entry and exit points
	ARGBColor		light;			// variable for light sources
	ARGBColor		shine;			// variable for shines
	ARGBColor		color;			// variable for pixel color
	CombineType		combineType;	// type of object combination
	segments_t		segments;		// container for segments handling
	Ray(void);
	~Ray(void);
	Ray(const Ray& other);
	Ray& operator=(const Ray& other);
	Ray& operator=(const RayBasic& raySafe);
	Ray& operator=(const ColorsSafe& colorsSafe);
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
			pow = dir;
	}
	inline void collectLight(const ARGBColor& sceneryColor, float k = 1) {
		color.addition(color, light * sceneryColor * k);
	}
	inline void collectShine(int specular, float d = 1.) {
		if (specular != -1 && d > 0.) {
			float k = dirL.get_reflect(norm) * pow;
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
