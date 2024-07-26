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

class	A_Scenery;
class	MlxImage;
class	Camera;
class	Lighting;

struct Scene {
	const std::vector<std::string> nicks {
		"bc","R","A","c","l","sp"
	};
	MlxImage&				img;
	std::vector<A_Scenery*>	scenerys;
	std::vector<A_Scenery*>	objsIdx;
	std::vector<A_Scenery*>	lightsIdx;
	std::vector<Camera>		cameras;
private:
	Position				_base;
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
	bool set_any(std::istringstream is);
	void set_scenery(A_Scenery* scenery);
	int	 parsing(int ac, char** av);
	void indexingScenerys(void);
	void initLoockats(void);
	bool checkCameraIdx(int cameraIdx) const;
	void recalculateLookatsForCurrentCamera(const Position& eye);
	void raytrasingCurrentCamera(void);
	A_Scenery* intersection(Ray& ray, int cam, float roll);
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
};


#endif /* SCENE_HPP */
