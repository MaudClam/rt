//
//  Scene.hpp
//  rt
//
//  Created by uru on 12/07/2024.
//

#ifndef SCENE_HPP
# define SCENE_HPP

# include "Header.h"

class AScenery;
class MlxImage;
class Camera;

struct Scene {
	MlxImage&				img;
	std::vector<AScenery*>	scenerys;
	std::vector<AScenery*>	objsIdx;
	std::vector<AScenery*>	lightsIdx;
	std::vector<Camera>		cameras;
	int						currentCamera;
	Scene(MlxImage& img);
	~Scene(void);
	Scene(const Scene& other);
	Scene& operator=(const Scene& other);
	void set_scenery(AScenery* scenery);
	void indexing(void);
	void set_camera(const Camera& camera);
	void set_camera(const MlxImage& img, std::istringstream& is);
	void initCameras(void);
	void makeLookatsPositions(void);
//	void init(int ac, char** av);
	void rt(void);
	void putPixelsToImg(void);
	void nextCamera(void);
	void previousCamera(void);
	void chooseCamera(int i);
};

#endif /* SCENE_HPP */
