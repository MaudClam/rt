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
struct	RaySafe;
struct	ColorsSafe;
struct	Ray;

struct RaySafe {
	Vec3f	pov;		// POV (point of view)
	Vec3f	dir;		// normalized ray direction vector
	Vec3f	dirFromCam;	// normalized camera direction vector
	Vec3f	dirToLight;	// normalized direction vector to light source
	Vec3f	norm;		// normalized normal vector from the ray hit point
	float	dist;		// distance from pov to object hit point
	RaySafe(void);
	RaySafe(const Ray& ray);
	~RaySafe(void);
	RaySafe(const RaySafe& other);
	RaySafe& operator=(const RaySafe& other);
	RaySafe& operator=(const Ray& ray);
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

struct Ray : public RaySafe {
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
	};
	struct Segment {
		Point a;
		Point b;
		bool  removed;
		Segment(void) : a(), b() {}
		Segment(const Point& a, const Point& b) : a(a), b(b), removed(false) {}
		Segment(float ad, bool ai, A_Scenery* as, float bd, bool bi, A_Scenery* bs, bool r) :
		a(ad,ai,as), b(bd,bi,bs), removed(r) {}
		Segment(const Segment& other) : a(other.a), b(other.b), removed(other.removed) {}
		~Segment(void) {}
		Segment& operator=(const Segment& other) {
			if (this != &other) {
				a = other.a;
				b = other.b;
				removed = other.removed;
			}
			return *this;
		}
		Segment& activate(A_Scenery* scenery) {
			if (scenery) {
				a.inside = false;
				b.inside = a.d == b.d ? false : true;
			}
			a.s = scenery;
			b.s = scenery;
			removed = false;
			return *this;
		}
	};
	int				recursion;		// current recursion number
	Segment			intersections;	// segment on ray - object entry and exit points
	ARGBColor		light;			// variable for light sources
	ARGBColor		shine;			// variable for shines
	ARGBColor		color;			// variable for pixel color
	Hit				hit;			// type of contact with an object
	CombineType		t1, t2;			// type of object combination
	segments_t		segments;		// container for segments handling
	Ray(void);
	~Ray(void);
	Ray(const Ray& other);
	Ray& operator=(const Ray& other);
	Ray& operator=(const RaySafe& raySafe);
	Ray& restore(const RaySafe& raySafe);
	Ray& set_hit(Hit hit);
	void combineStart(A_Scenery* scenery, Hit targetHit);
	void combineNext(A_Scenery* scenery, Hit targetHit);
	void combination(CombineType type);
	void union_(Segment& segment1, Segment& segment2);
	void subtraction(Segment& segment1, Segment& segment2);
	void intersection(Segment& segment1, Segment& segment2);
	inline void emplace(const Segment& segment) {
		emplace(segment.a, segment.b);
	}
	inline void emplace(const Point& a, const Point& b) {
		segments.emplace_front(a.d, a.inside, a.s, b.d, b.inside, b.s, false);
	}
	inline void firstVisible(void) {}
	inline void secondVisible(void) {
		segments.clear();
		emplace(intersections);
	}
	inline void noVisible(void) {
		segments.clear();
	}
	inline bool first(void) {
		auto segment = segments.begin(), end = segments.end();
		while (segment != end && segment->removed) {
			++segment;
		}
		return segment != end; }
	inline bool second(void) { return intersections.a.s; }
	inline A_Scenery* combineGet(void) {
		return  combineGet(intersections.a.set(_INFINITY, false, NULL));
	}
	inline A_Scenery* combineGet(Point& nearest) {
		for (auto segment = segments.begin(), end = segments.end(); segment != end; ++segment) {
			if (segment->removed) {
				continue;
			}
			if (nearest.d > segment->a.d && segment->a.d >= 0) {
				nearest = segment->a;
			}
			if (nearest.d > segment->b.d && segment->b.d >= 0) {
				nearest = segment->b;
			}
		}
		if (nearest.s) {
			dist = nearest.d;
			hit = nearest.inside ? INSIDE : OUTSIDE;
		}
		return nearest.s;
	}
	inline void movePovByDirToDist(void) {
		pov.addition( pov, dir * dist );
	}
	inline void movePovByNormal(float distance) {
		pov.addition(pov, norm * distance);
	}
	inline void fixDirFromCam_if(void) {
		if (!recursion) {
			dirFromCam = dir;
		}
	}
	inline void collectLight(const ARGBColor& sceneryColor, float k = 1) {
		color.addition(color, light * sceneryColor * k);
	}
	inline void collectShine(int specular) {
		if (specular != -1) {
			float k = dirToLight.reflect(norm) * dirFromCam;
			if (k > 0) {
				k = std::pow(k, specular);
				shine.addition(shine, light * k);
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
	inline void collectShadowLight(const ColorsSafe& colorsSafe, const ARGBColor& sceneryColor, float k) {
		light = color;
		color.val = colorsSafe.color;
		collectLight(sceneryColor, k);
		shine.val = colorsSafe.shine;
		light.val = colorsSafe.light;
	}
};


#endif /* RAY_HPP */
