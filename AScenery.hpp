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

struct	Ray;
class	Camera;

class AScenery {
protected:
	std::string				_name;
	std::string				_nick;
	bool					_light;
	float					_brightness;
	Position				_pos;
public:
	enum Side { FRONT, BACK };
	std::vector<Position>	lookats;
	ARGBColor				color;
	AScenery(void);
	virtual ~AScenery(void);
	AScenery(const AScenery& other);
	std::string  get_nick(void) const;
	bool  get_light(void) const;
	float get_brightness(void) const;
	Position get_pos(void) const;
	void set_brightness(float brightness);
	void set_pos(const Position& pos);
	bool checkLookatsIdx(int idx) const;
	void set_lookatCamera(const Position& eye, const LookatAux& aux);
	void set_lookatBase(void);
	void recalculateLookat(int idx, const Position& eye, const LookatAux& aux);
	virtual bool intersection(Ray& ray, int cameraIdx, float cameraRoll = 0, Side side = FRONT) const = 0;
};

class BasicCoordinate : public AScenery {
public:
	BasicCoordinate(void);
	~BasicCoordinate(void);
	BasicCoordinate(const BasicCoordinate& other);
	BasicCoordinate& operator=(const BasicCoordinate& other);
	bool intersection(Ray& ray, int cameraIdx, float cameraRoll = 0, Side side = FRONT) const;
};

#endif /* ASCENERY_HPP */
