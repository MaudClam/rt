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
	Light(void);
	~Light(void);
	Light(const Light& other);
	Light* clone(void) const;
	void set_nick(const std::string& nick);
	void set_name(const std::string& name);
	void set_type(Type type);
	void lookat(const Position& eye, const LookatAux& aux, const Vec3f& pos, float roll);
	void roll(const Vec3f& pos, float shiftRoll);
	bool intersection(Ray& ray) const;
	void giveNormal(Ray& ray) const;
	float getDistanceToShaderEdge(Ray& ray, float distance, bool inside) const;
	float lighting(Ray& ray) const;
	void photonEmission(int num, DirMatrix& dirs, photonRays_t& rays) const;
	virtual void output(std::ostringstream& os) const;
	friend std::ostream& operator<<(std::ostream& o, const Light& sp);
	friend std::istringstream& operator>>(std::istringstream& is, Light& sp);
};

#endif /* LIGHT_HPP */
