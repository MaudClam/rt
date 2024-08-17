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

class A_Scenery {
protected:
	std::string	_name;
	std::string	_nick;
	bool		_isLight;
	Position	_pos;
public:
	ARGBColor	color;
	int			specular;	// in range [-1,1000]
	float		reflective;	// in range [0,1]
	float		refractive; // u in range [0,1]
	float		n;			// refractive index relative to air
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
	virtual float lighting(Ray& ray, const A_Scenery& scenery, const a_scenerys_t& scenerys) const = 0;
	virtual void output(std::ostringstream& os) = 0;
	friend std::ostream& operator<<(std::ostream& o, A_Scenery& s);
};

#endif /* A_SCENERY_HPP */
