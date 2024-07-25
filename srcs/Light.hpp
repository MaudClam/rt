//
//  Lighting.hpp
//  rt
//
//  Created by uru on 25/07/2024.
//

#ifndef LIGHT_HPP
#define LIGHT_HPP

# include "Header.h"

class	AScenery;
class	Camera;
struct	Ray;
struct	Position;
class	Lighting;

class Light : public AScenery {
	Lighting	light;
public:
	Light(void);
	~Light(void);
	Light(const Light& other);
	Light& operator=(const Lighting& other);
	bool intersection(Ray& ray, int cameraIdx, float cameraRoll = 0, Side side = FRONT) const;
	void lighting(Ray& ray, int cameraIdx, float cameraRoll = 0) const;
	void hit(Ray& ray, int cameraIdx, float cameraRoll = 0) const;
	friend std::ostream& operator<<(std::ostream& o, Light& sp);
	friend std::istringstream& operator>>(std::istringstream& is, Light& sp);
};

#endif /* LIGHT_HPP */
