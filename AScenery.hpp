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

class AScenery {
protected:
	std::string	name;
	std::string	nick;
	bool		light;
	float		brightness;
	Vec3f		center;
	ARGBColor	color;
public:
	AScenery(void);
	virtual ~AScenery(void);
	AScenery(const AScenery& other);
	virtual AScenery& operator=(const AScenery& other);
	virtual bool intersection(Ray& ray) const = 0;
};

#endif /* ASCENERY_HPP */
