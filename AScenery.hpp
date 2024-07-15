//
//  AScenery.hpp
//  rt
//
//  Created by uru on 09/07/2024.
//

#ifndef ASCENERY_HPP
# define ASCENERY_HPP

# include "geometry.hpp"
# include "ARGBColor.hpp"
# include "Ray.hpp"

class Camera;

class AScenery {
protected:
	std::string	name;
	std::string	nick;
	bool		light;
	float		brightness;
	Vec3f		center;
	Vec3f		normal;
public:
	ARGBColor	color;
	AScenery(void);
	virtual ~AScenery(void);
	AScenery(const AScenery& other);
	virtual AScenery& operator=(const AScenery& other);
	bool get_light(void);
	virtual bool intersection(Ray& ray) const = 0;
	virtual bool intersection(Ray& ray, int currentCamera) const = 0;
	virtual void calculateLookatForCamera(const Position& cameraPosition) = 0;
};

#endif /* ASCENERY_HPP */
