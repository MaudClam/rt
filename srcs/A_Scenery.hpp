//
//  A_Scenery.hpp
//  rt
//
//  Created by uru on 09/07/2024.
//

#ifndef ASCENERY_HPP
# define ASCENERY_HPP

# include "geometry.hpp"
# include "ARGBColor.hpp"
# include "Ray.hpp"

class	A_Scenery;
typedef std::vector<A_Scenery*> a_scenerys_t;
typedef a_scenerys_t::iterator	a_scenerys_it;

struct Combine {
	Ray&			ray;
	float			d1[3];
	float			d2[3];
	A_Scenery*		s1;
	A_Scenery*		s2;
	Hit				hit, h1, h2;
	CombineType		t1, t2;
	Combine(Ray& ray, A_Scenery* scenery, Hit hit);
	~Combine(void);
	Combine(const Combine& other);
	Combine& operator=(const Combine& other);
	Combine& nextPrimitive(A_Scenery* scenery);
	void chooseSurface(void);
	void subtraction12(void);
	void intersection(void);
	inline void swap(void) {
		for (int i = 0; i < 3; ++i)
			std::swap(d1[i],d2[i]);
		std::swap(s1,s2);
		std::swap(h1,h2);
	}
	inline void shift2to1(void) {
		for (int i = 0; i < 3; ++i)
			d1[i] = d2[i];
		s1 = s2;
		h1 = h2;
	}
	inline void visible1(void) {}
	inline void visible2(void) { shift2to1(); }
	inline void noVisible(void) { s1 = NULL; }
	inline bool d11inside2(void) { return d2[1] <= d1[1] && d1[1] <= d2[2]; }
	inline bool d12inside2(void) { return d2[1] <= d1[2] && d1[2] <= d2[2]; }
	inline bool d21inside1(void) { return d1[1] <= d2[1] && d2[1] <= d1[2]; }
	inline bool d22inside1(void) { return d1[1] <= d2[2] && d2[2] <= d1[2]; }
	inline bool _1inside2(void) { return d11inside2() && d12inside2(); }
	inline bool _2inside1(void) { return d21inside1() && d22inside1(); }
	inline bool _notIntersect(void) {
		return !d11inside2() && !d12inside2() && !d21inside1() && !d22inside1();
	}
	inline bool _12intrsect(void) { return d21inside1() && !d22inside1(); }
	inline bool _21intrsect(void) { return d11inside2() && !d12inside2(); }
	inline A_Scenery* get_result(void) {
		ray.dist = d1[0];
		ray.d1 = d1[1];
		ray.d2 = d1[2];
		ray.hit = h1;
		return s1;
	}
};

class A_Scenery {
protected:
	std::string		_name;
	std::string		_nick;
	bool			_isLight;
	Position		_pos;
public:
	CombineType		combineType;// combination type with the following primitive
	ARGBColor		color;		// surface color
	int				specular;	// in range [-1,1000]
	float			reflective;	// in range [0,1]
	float			refractive; // in range [0,1]
	float			a_matIOR;	// refractive index relative to air
	A_Scenery(void);
	virtual ~A_Scenery(void);
	A_Scenery(const A_Scenery& other);
	std::string  get_nick(void) const;
	bool  get_isLight(void) const;
	Position get_pos(void) const;
	void set_pos(const Position& pos);
	virtual A_Scenery* clone(void) const = 0;
	virtual void lookat(const Position& eye, const LookatAux& aux, const Vec3f& pov, float roll) = 0;
	virtual void roll(const Vec3f& pov, float shiftRoll) = 0;
	virtual bool intersection(Ray& ray) const = 0;
	virtual void calculateNormal(Ray& ray) const = 0;
	virtual float lighting(Ray& ray) const = 0;
	virtual void output(std::ostringstream& os) = 0;
	friend std::ostream& operator<<(std::ostream& o, A_Scenery& s);
};

#endif /* A_SCENERY_HPP */
