//
//  Ray.hpp
//  rt
//
//  Created by uru on 01/08/2024.
//

#ifndef RAY_HPP
#define RAY_HPP

# include <list>
# include "geometry.hpp"
# include "ARGBColor.hpp"
# include "A_Scenery.hpp"

struct	Combine;
class	A_Scenery;

struct Ray {
	int			recursion;	// recursion counter
	Vec3f		pov;		// POV (point of view)
	Vec3f		dir;		// normalized ray direction vector
	Vec3f		dirFromCam;	// normalized camera direction vector
	Vec3f		dirToLight;	// normalized direction vector to light
	Vec3f		norm;		// normalized normal vector from the ray hit point
	float		dist,d1,d2;	// distance from pov to object
	ARGBColor	light;
	ARGBColor	shine;
	ARGBColor	color;
	Hit			hit;
	Ray(void);
	~Ray(void);
	Ray(const Ray& other);
	Ray& operator=(const Ray& other);
	Ray& set_hit(Hit hit);
	Ray& changePov(void);
	Ray& movePovByNormal(float distance);
	Ray& partRestore(const Ray& other);
	Ray& collectLight(const ARGBColor& sceneryColor, float k = 1);
	Ray& collectShine(int specular);
	Ray& collectReflectiveLight(int _color, int _shine, float reflective);
	Ray& collectRefractiveLight(const ARGBColor& sceneryColor, int _color, float refractive);
};

struct Combine {
	struct	Point;
	struct	Segment;
	typedef	std::list<Segment>			segments_t;
	typedef	segments_t::iterator		segments_it;
	typedef	std::vector<segments_it>	segmForDel_t;
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
		Point& replace(float _d, bool _inside, A_Scenery* _s) {
			d = _d;
			inside = _inside;
			s = _s;
			return *this;
		}
	};
	struct Segment {
		Point a;
		Point b;
		Segment(void) : a(), b() {}
		Segment(const Point& a, const Point& b) : a(a), b(b) {}
		Segment(float ad, bool ai, A_Scenery* as, float bd, bool bi, A_Scenery* bs) : a(ad,ai,as), b(bd,bi,bs) {}
		Segment(const Segment& other) : a(other.a), b(other.b) {}
		~Segment(void) {}
		Segment& operator=(const Segment& other) {
			if (this != &other) {
				a = other.a;
				b = other.b;
			}
			return *this;
		}
	};
	Ray&			ray;
	Hit				hit;
	CombineType		t1, t2;
	segments_t		segments1;
	segmForDel_t	del;
	Segment			segment2;
	Combine(Ray& ray, A_Scenery* scenery, Hit hit);
	~Combine(void);
	Combine(const Combine& other);
	Combine& operator=(const Combine& other);
	void emplace(const Segment& segment);
	void emplace(const Point& a, const Point& b);
	void next(A_Scenery* scenery);
	void combination(CombineType type);
	bool union_(segments_it& segment1, Segment& segment2);
	bool subtraction(segments_it& segment1, Segment& segment2);
	bool intersection(segments_it& segment1, Segment& segment2);
	inline void firstVisible(void) {}
	inline void secondVisible(void) {
		segments1.clear();
		segments1.push_front(segment2);
	}
	inline void noVisible(void) {
		segments1.clear();
	}
	inline bool first(void) { return !segments1.empty(); }
	inline bool second(void) { return segment2.a.s; }
	inline A_Scenery* get(void) {
		return  get(segment2.a.replace(_INFINITY, false, NULL));
	}
	inline A_Scenery* get(Point& nearest) {
		for (auto segment = segments1.begin(), end = segments1.end(); segment != end; ++segment) {
			if (nearest.d > segment->a.d && segment->a.d >= 0) {
				nearest = segment->a;
			}
			if (nearest.d > segment->b.d && segment->b.d >= 0) {
				nearest = segment->b;
			}
		}
		if (nearest.s) {
			ray.dist = nearest.d;
			ray.hit = nearest.inside ? INSIDE : OUTSIDE;
		}
		return nearest.s;
	}
};

#endif /* RAY_HPP */
