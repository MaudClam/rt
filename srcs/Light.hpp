//
//  Lighting.hpp
//  rt
//
//  Created by uru on 25/07/2024.
//

#ifndef LIGHT_HPP
#define LIGHT_HPP

# include "Header.h"

class	A_Scenery;
class	Camera;
struct	Ray;
struct	Position;
class	Lighting;

class Light : public A_Scenery {
	Lighting	light;
public:
	Light(void);
	~Light(void);
	Light(const Light& other);
	Light& operator=(const Lighting& other);
	bool intersection(Ray& ray, int cameraIdx, Side side = FRONT) const;
	void hit(Ray& ray, int cameraIdx) const;
	void lighting(Ray& ray, int cameraIdx) const;
	virtual void output(std::ostringstream& os);
	friend std::ostream& operator<<(std::ostream& o, Light& sp);
	friend std::istringstream& operator>>(std::istringstream& is, Light& sp);
};

#endif /* LIGHT_HPP */
