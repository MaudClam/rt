//
//  Camera.hpp
//  rt
//
//  Created by uru on 13/07/2024.
//

#ifndef CAMERA_HPP
# define CAMERA_HPP

# include "Header.h"

class Fov {
protected:
	float _degree;
	float _tan;
public:
	Fov(void);
	~Fov(void);
	Fov(const Fov& other);
	Fov& operator=(const Fov& other);
	float get_degree(void) const;
	float get_tan(void) const;
	bool set_degree(float degree);
	bool set_tan(float tan);
	friend std::ostream& operator<<(std::ostream& o, Fov& fov);
	friend std::istringstream& operator>>(std::istringstream& is, Fov& fov);
};

struct Pixel {
	std::vector<Ray>	rays;
	Vec3f				cPos; // relative xy-coordinate on RT canvas of width 1
	ARGBColor			color;
	Pixel(const Vec3f& cPos, int smoothingFactor, float tan, const Vec3f& pov);
	~Pixel(void);
	Pixel(const Pixel& other);
	Pixel& operator=(const Pixel& other);
	void reset(int smoothingFactor, float tan, const Vec3f& pov);
	void restoreRays(int smoothingFactor, float tan, const Vec3f& pov);
	void averageColor(void);
};
	
class Matrix {
protected:
	int		_width;
	int		_height;
	int		_bytespp;
	float	_mult;
	Fov		_fov;
	int		_sm;
public:
	std::vector<Pixel>	matrix;
	Matrix(void);
	~Matrix(void);
	Matrix(const Matrix& other);
	Matrix& operator=(const Matrix& other);
	int  get_width(void);
	int  get_height(void);
	int  get_bytespp(void);
	Fov  get_fov(void);
	float get_fovDegree(void);
	float get_fovTan(void);
	int   get_sm(void);
	bool  set_fovDegree(float degree);
};

class Camera : public Matrix {
protected:
	const std::string	_name = "camera";
	const std::string	_nick = "c";
	Position			_base;
	Position			_pos;
	float				_roll;	// Camera tilt (aviation term 'roll') relative to its optical axis (z-axis)
	float				_flybyRadius;
public:
	a_scenerys_t		scenerys;
	a_scenerys_t		objsIdx;
	a_scenerys_t		lightsIdx;
	ARGBColor 			ambient, space;
	Camera(const MlxImage& img);
	~Camera(void);
	Camera(const Camera& other);
	Camera& operator=(const Camera& other);
	Position get_pos(void) const;
	float	get_rollDegree(void) const;
	float	get_roll(void) const;
	float	get_flybyRadius(void) const;
	int		get_sm(void) const;
	void	set_scenery(A_Scenery* scenery);
	void	set_pos(const Position& pos);
	void	set_posToBase(void);
	void	set_flybyRadius(float flybyRadius);
	void	initMatrix(void);
	void	restoreRays_lll(unsigned long begin, unsigned long end);
	static void restoreRays(Camera* camera, unsigned long begin, unsigned long end);
	void	resetRays_lll(unsigned long begin, unsigned long end);
	static void resetRays(Camera* camera, unsigned long begin, unsigned long end);
	bool	resetFovDegree(float degree);
	void	resetSmoothingFactor(int smoothingFactor);
	void	resetRoll(float roll);
	void	lookatCamera(const Position& pos);
	void	takePicture_lll(MlxImage& img, unsigned long begin, unsigned long end);
	static void	takePicture(Camera* camera, MlxImage& img, unsigned long begin, unsigned long end);
	void	rayTracing_lll(unsigned long begin, unsigned long end);
	static void	rayTracing(Camera* camera, unsigned long begin, unsigned long end);
	void	traceRay(Ray& ray);
	A_Scenery* closestScenery(Ray& ray);
	void	calculateFlybyRadius(void);
	friend	std::ostream& operator<<(std::ostream& o, Camera& camera);
	friend	std::istringstream& operator>>(std::istringstream& is, Camera& camera);
};

// Non member functions

#endif /* CAMERA_HPP */
