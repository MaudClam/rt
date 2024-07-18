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
	std::string				name;
	std::string				nick;
	bool					light;
	float					brightness;
	Position				pos;
	std::vector<Position>	lookats;
public:
	ARGBColor				color;
	AScenery(void);
	virtual ~AScenery(void);
	AScenery(const AScenery& other);
	bool checkLookatsIdx(int idx) const;
	bool get_light(void) const;
	void set_lookat(const Position& eye, float roll);
	void recalculateLookat(int idx, const Position& eye, float roll);
	virtual bool intersection(Ray& ray) const = 0;
	virtual bool intersection(Ray& ray, int cameraIdx) const = 0;
};

#endif /* ASCENERY_HPP */
