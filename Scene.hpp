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
	bool set_currentCamera(int cameraIdx);
	int	 parsing(int ac, char** av);
	void set_scenery(AScenery* scenery);
	void set_camera(const Camera& camera);
	void set_camera(std::istringstream is);
	void indexingScenerys(void);
	void initCameras(void);
	void resetCurrentCamera(void);
	bool checkCameraIdx(int cameraIdx) const;
	void recalculateLookatsForCurrentCamera(void);
	void raytraisingCurrentCamera(void);
	void rt(void);
	void putCurrentCameraPixelsToImg(void);
	void selectCurrentCamera(int ctrl);
	void changeCurrentCameraFOV(int ctrl);
	void moveCurrentCamera(int ctrl);
	void rotateCurrentCamera(int ctrl);
	void setFlybyRadiusForCurrentCamera(void);
};

#endif /* SCENE_HPP */
