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
	bool checkLookatsIdx(int idx) const;
	void set_lookatCamera(const Position& eye, const LookatAux& aux);
	void set_lookatBase(void);
	void recalculateLookat(int idx, const Position& eye, const LookatAux& aux);
	void recalculateLookat(int idx, float roll);
	bool intersection(Ray& ray, int cameraIdx, Hit rayHit = FRONT) const;
	void getNormal(Ray& ray, int cameraIdx) const;
	bool lighting(Ray& ray, int cameraIdx) const;
	virtual void output(std::ostringstream& os);
	friend std::ostream& operator<<(std::ostream& o, Light& sp);
	friend std::istringstream& operator>>(std::istringstream& is, Light& sp);
};

#endif /* LIGHT_HPP */
