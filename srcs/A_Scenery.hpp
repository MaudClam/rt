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
	int						specular;	// in range [-1,1000]
	float					reflective;	// in range [0,1]
	A_Scenery(void);
	virtual ~A_Scenery(void);
	A_Scenery(const A_Scenery& other);
	std::string  get_nick(void) const;
	bool  get_isLight(void) const;
	Position get_pos(void) const;
	void set_pos(const Position& pos);
	virtual bool checkLookatsIdx(int idx) const = 0;
	virtual void set_lookatCamera(const Position& eye, const LookatAux& aux) = 0;
	virtual void set_lookatBase(const Position& eye) = 0;
	virtual void recalculateLookat(int cameraIdx, const Position& eye, const LookatAux& aux) = 0;
	virtual void recalculateLookat(int cameraIdx, float roll, const Vec3f& newPov) = 0;
	virtual bool intersection(Ray& ray, int cameraIdx, Hit rayHit = FRONT) const = 0;
	virtual void getNormal(Ray& ray, int cameraIdx) const = 0;
	virtual bool lighting(Ray& ray, int cameraIdx) const = 0;
	virtual void output(std::ostringstream& os) = 0;
	friend std::ostream& operator<<(std::ostream& o, A_Scenery& s);
};

#endif /* A_SCENERY_HPP */
