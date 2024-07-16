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
private:
	int						currentCamera;
public:
	Scene(MlxImage& img);
	~Scene(void);
	Scene(const Scene& other);
	Scene& operator=(const Scene& other);
	int  get_currentCamera(void);
	void set_currentCamera(int cameraIdx);
	int	 parsing(int ac, char** av);
	void set_scenery(AScenery* scenery);
	void set_camera(const Camera& camera);
	void set_camera(std::istringstream is);
	void indexingScenerys(void);
	void initCameras(void);
	void resetCamera(int cameraIdx);
	bool checkCameraIdx(int cameraIdx) const;
	void recalculateLookatsForCurrentCamera(void);
	void raytraisingCurrentCamera(void);
	void rt(void);
	void putCurrentCameraPixelsToImg(void);
	void nextCamera(void);
	void previousCamera(void);
	void chooseCamera(int i);
};

#endif /* SCENE_HPP */
