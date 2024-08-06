//
//  Lighting.hpp
//  rt
//
//  Created by uru on 25/07/2024.
//

#ifndef LIGHT_HPP
#define LIGHT_HPP

# include "A_Scenery.hpp"

class Light : public A_Scenery {
public:
	enum Type { SPOTLIGHT, SUNLIGHT, SUNLIGHT_LIMITED };
protected:
	Type     _type;
public:
	Lighting light;
	Light(void);
	~Light(void);
	Light(const Light& other);
	Light* clone(void) const;
	void set_nick(const std::string& nick);
	void set_name(const std::string& name);
	void set_type(Type type);
	void lookat(const Position& eye, const LookatAux& aux, const Vec3f& pov);
	bool intersection(Ray& ray, bool notOptimize = true, Hit rayHit = FRONT) const;
	void getNormal(Ray& ray) const;
	bool lighting(Ray& ray) const;
	virtual void output(std::ostringstream& os);
	friend std::ostream& operator<<(std::ostream& o, Light& sp);
	friend std::istringstream& operator>>(std::istringstream& is, Light& sp);
};

#endif /* LIGHT_HPP */
