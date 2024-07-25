//
//  Scene.hpp
//  rt
//
//  Created by uru on 12/07/2024.
//

#ifndef SCENE_HPP
# define SCENE_HPP

# include "Header.h"

# define RECURSIONS	5

class	AScenery;
class	MlxImage;
class	Camera;
class	Lighting;

struct Scene {
	const Position			base;
	const Vec3f				nullVec;
	MlxImage&				img;
	std::vector<AScenery*>	scenerys;
	std::vector<AScenery*>	objsIdx;
	std::vector<AScenery*>	lightsIdx;
	std::vector<Camera>		cameras;
private:
	Vec2i					_resolution;
	std::string				_header;
	Lighting				_ambient;
	Lighting				_space;
	int						_currentCamera;
public:
	Scene(MlxImage& img);
	~Scene(void);
	Scene(const Scene& other);
	Scene& operator=(const Scene& other);
	int  get_currentCamera(void);
	bool set_currentCamera(int cameraIdx);
	void set_scenery(AScenery* scenery);
	void set_ambientLighting(std::istringstream is);
	void set_camera(const Camera& camera);
	void set_camera(std::istringstream is);
	int	 parsing(int ac, char** av);
	void indexingScenerys(void);
	void initLoockats(void);
	bool checkCameraIdx(int cameraIdx) const;
	void recalculateLookatsForCurrentCamera(const Position& eye);
	void raytrasingCurrentCamera(void);
	AScenery* intersection(Ray& ray, int cam, float roll);
	bool shadow(Ray& ray, int cam, float roll);
	void trasingRay(Ray& ray, int cam, float roll);
	void rt(void);
	void selectCurrentCamera(int ctrl);
	void changeCurrentCameraFOV(int ctrl);
	void moveCurrentCamera(int ctrl);
	void rotateCurrentCamera(int ctrl);
	void setFlybyRadiusForCurrentCamera(void);
	void flybyCurrentCamera(void);
	friend std::ostream& operator<<(std::ostream& o, Scene& sc);
	friend std::istringstream& operator>>(std::istringstream& is, Scene& sc);
};


#endif /* SCENE_HPP */
