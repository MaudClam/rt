//
//  Scene.hpp
//  rt
//
//  Created by uru on 12/07/2024.
//

#ifndef SCENE_HPP
# define SCENE_HPP

# include "Header.h"

class	A_Scenery;
class	MlxImage;
class	Camera;
typedef std::vector<Camera>	cameras_t;

struct Scene {
	const std::vector<std::string> nicks {
		"R","A","c","l","ls","ll","sp"
	};
	MlxImage&		img;
	a_scenerys_t	scenerys;
	a_scenerys_t	objsIdx;
	a_scenerys_t	lightsIdx;
	cameras_t		cameras;
private:
	Vec2i			_resolution;
	std::string		_header;
	Lighting		_ambient;
	Lighting		_space;
	int				_currentCamera;
public:
	Scene(MlxImage& img);
	~Scene(void);
	Scene(const Scene& other);
	Scene& operator=(const Scene& other);
	int	 parsing(int ac, char** av);
	int  get_currentCamera(void);
	bool set_currentCamera(int idx);
	bool set_any(std::istringstream is);
	void set_scenery(A_Scenery* scenery);
	void makeLookatsForCameras(void);
	A_Scenery* nearestIntersection(Ray& ray);
	void rt(void);
	void selectCamera(int ctrl);
	void changeCameraFOV(int ctrl);
	void moveCamera(int ctrl);
	void rotateCamera(int ctrl);
	void flybyCamera(void);
	friend std::ostream& operator<<(std::ostream& o, Scene& sc);
};


#endif /* SCENE_HPP */
