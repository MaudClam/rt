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

struct Ray {
	Vec3f		pov;	// POV (point of view)
	Vec3f		dir;	// ray direction vector (normalized)
	Vec3f		norm;	// normal vector at the ray hit (normalized)
	float		dist;	// vdistance from pov to object
	int			hits;	// number of ray reflections, refractions, absorptions
	ARGBColor	tmpColor;
	ARGBColor	color;
	Ray(void);
	~Ray(void);
	Ray(const Position& pos);
	Ray(const Ray& other);
	Ray& operator=(const Ray& other);
};

struct Pixel {
	Ray			ray;
	Vec2f		cPos;	// relative xy-coordinate on RT canvas of width 1
	Pixel(const Vec2f& cPos, float tan, const Vec3f& pov);
	~Pixel(void);
	Pixel(const Pixel& other);
	Pixel& operator=(const Pixel& other);
	void reset(float tan, const Vec3f& pov);
};
	
class Matrix {
protected:
	int		_width;
	int		_height;
	int		_bytespp;
	float	_mult;
	Fov		_fov;
public:
	std::vector<Pixel> matrix;
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
	void set_width(int width);
	void set_height(int height);
	void set_bytespp(int bytespp);
	bool set_fovTan(float tan);
	bool set_fovDegree(float degree);
};

class Camera : public Matrix {
protected:
	const std::string	_name = "camera";
	const std::string	_nick = "c";
	Position			_pos;
	Position			_pos0;
	float				_roll;
	float				_flybyRadius;
public:
	Camera(const MlxImage& img, const Position& base);
	~Camera(void);
	Camera(const MlxImage& img, const Position& pos, float fov, const Position& base);
	Camera(const Camera& other);
	Camera& operator=(const Camera& other);
	Position	get_pos(void) const;
	Position	get_pos0(void) const;
	float		get_rollDegree(void) const;
	float		get_roll(void) const;
	float		get_flybyRadius(void) const;
	void		set_pos(const Position& pos);
	void		set_pos0(const Position& pos0);
	void		set_flybyRadius(float flybyRadius);
	void		initMatrix(void);
	void		resetMatrix(void);
	void		takePicture(MlxImage& img);
	bool		reset_fovDegree(float degree);
	void		reset_pov(const Position& pos0);
	void		reset_roll(float roll);
	friend std::ostream& operator<<(std::ostream& o, Camera& camera);
	friend std::istringstream& operator>>(std::istringstream& is, Camera& camera);
};

// Non member functions

#endif /* CAMERA_HPP */
