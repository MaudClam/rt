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

struct	Ray;
class	Camera;

class A_Scenery {
protected:
	std::string				_name;
	std::string				_nick;
	bool					_isLight;
	Position				_pos;
public:
	std::vector<Lookat>		lookats;
	ARGBColor				color;
	A_Scenery(void);
	virtual ~A_Scenery(void);
	A_Scenery(const A_Scenery& other);
	std::string  get_nick(void) const;
	bool  get_isLight(void) const;
	Position get_pos(void) const;
	void set_pos(const Position& pos);
	virtual bool checkLookatsIdx(int idx) const = 0;
	virtual void set_lookatCamera(const Position& eye, const LookatAux& aux) = 0;
	virtual void set_lookatBase(void) = 0;
	virtual void recalculateLookat(int idx, const Position& eye, const LookatAux& aux) = 0;
	virtual void recalculateLookat(int idx, float roll) = 0;
	virtual bool intersection(Ray& ray, int cameraIdx, Hit rayHit = FRONT) const = 0;
	virtual void getNormal(Ray& ray, int cameraIdx) const = 0;
	virtual bool lighting(Ray& ray, int cameraIdx) const = 0;
	virtual void output(std::ostringstream& os) = 0;
	friend std::ostream& operator<<(std::ostream& o, A_Scenery& s);
};

class BasicCoordinate : public A_Scenery {
public:
	BasicCoordinate(const Position& pos);
	~BasicCoordinate(void);
	BasicCoordinate(const BasicCoordinate& other);
	BasicCoordinate& operator=(const BasicCoordinate& other);
	bool checkLookatsIdx(int idx) const;
	void set_lookatCamera(const Position& eye, const LookatAux& aux);
	void set_lookatBase(void);
	void recalculateLookat(int idx, const Position& eye, const LookatAux& aux);
	void recalculateLookat(int idx, float roll);
	bool intersection(Ray& ray, int cameraIdx, Hit rayHit = FRONT) const;
	void getNormal(Ray& ray, int cameraIdx) const;
	bool lighting(Ray& ray, int cameraIdx) const;
	void output(std::ostringstream& os);
	friend std::ostream& operator<<(std::ostream& o, BasicCoordinate& bc);
	friend std::istringstream& operator>>(std::istringstream& is, BasicCoordinate& bc);
};

#endif /* A_SCENERY_HPP */
