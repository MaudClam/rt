#ifndef CAMERA_HPP
# define CAMERA_HPP

# include <thread>
# include "MlxImage.hpp"
# include "Ray.hpp"


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
	bool  set_degree(float degree);
	friend std::ostream& operator<<(std::ostream& o, const Fov& fov);
	friend std::istringstream& operator>>(std::istringstream& is, Fov& fov);
};


struct Pixel {
	Rays		rays;
	Vec3f		cPos; // relative xy-coordinate on RT canvas of width 1
	MeanRgb		paint;
	Pixel(const Vec3f& cPos, int smoothingFactor, float tan, const Vec3f& pos);
	~Pixel(void);
	Pixel(const Pixel& other);
	Pixel& operator=(const Pixel& other);
	void reset(int smoothingFactor, float tan, const Vec3f& pov);
	void restoreRays(int smoothingFactor, float tan, const Vec3f& pov);
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
	Fov   get_fov(void);
	float get_fovDegree(void);
	int   get_sm(void);
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
	Scenerys	scenerys;
	Scenerys	objsIdx;
	Scenerys	lightsIdx;
	PhotonMap	phMap;
	Lighting	ambient;
	Lighting	background;
	int			depth;
	int			paths;
	MapType		photonMap;
	TracingType	tracingType;
	bool		fakeAmbientLightOn;
	bool		directLightOn;
	Camera(const MlxImage& img);
	~Camera(void);
	Camera(const Camera& other);
	Camera& operator=(const Camera& other);
	Position get_pos(void) const;
	float	get_rollDegree(void) const;
	float	get_flybyRadius(void) const;
	void	set_scenery(A_Scenery* scenery);
	void	set_posToBase(void);
	void	initMatrix(void);
	void	restoreRays_lll(size_t begin, size_t end);
	static void restoreRays(Camera* camera, size_t begin, size_t end);
	void	resetRays_lll(size_t begin, size_t end);
	static void resetRays(Camera* camera, size_t begin, size_t end);
	bool	resetFovDegree(float degree);
	void	resetSmoothingFactor(int smoothingFactor);
	void	resetRecursionDepth(int depth);
	void	resetPathsPerRay(int key);
	void	changePhotonMap(MapType type);
	void	changeOther(Controls key);
	void	resetRoll(float roll);
	void	lookatCamera(const Position& pos);
	void	takePicture_lll(MlxImage& img, size_t begin, size_t end);
	static void	takePicture(Camera* camera, MlxImage& img, size_t begin, size_t end);
	void	raysTracing_lll(size_t begin, size_t end);
	static void	raysTracing(Camera* camera, size_t begin, size_t end);
	
	void	tarcing(Ray& ray, int r = 0);
	void	traceRay(Ray& ray, int r = 0);
	void	reflections(Ray& ray, HitRecord& rec, float fading, int r);
	void	refractions(Ray& ray, HitRecord& rec, float fading, int r);
	void	lightings(Ray& ray, HitRecord& rec);
	void	ambientLightPathsTarcing(Ray& ray, HitRecord& rec, int r);
	void	tracePath(Ray& ray, int r = 0);
	void	ambientLightPath(Ray& ray, HitRecord& rec, int r);
	void	calculateFlybyRadius(void);
	void 	runThreadRoutine(int routine, MlxImage* img = NULL);

	friend	std::ostream& operator<<(std::ostream& o, const Camera& camera);
	friend	std::istringstream& operator>>(std::istringstream& is, Camera& camera);
};


#endif /* CAMERA_HPP */
