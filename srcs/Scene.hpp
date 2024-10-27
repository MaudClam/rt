//
//  Scene.hpp
//  rt
//
//  Created by uru on 12/07/2024.
//

#ifndef SCENE_HPP
# define SCENE_HPP

# include <fstream>
# include "Header.h"

class	A_Scenery;
class	MlxImage;
class	Camera;
typedef std::vector<std::string>	stringSet_t;
typedef std::vector<float>			floatSet_t;
typedef std::vector<Camera>			cameras_t;

enum MsgType {
	WELLCOM_MSG,
	WRNG_FILE_MISSING,
	ERR_OPEN_FILE,
	WRNG_RESOLUTION,
	WRNG_PARSING_ERROR,
	WRNG_PARSING_ERROR1,
	WRNG_PARSING_ERROR2,
	WRNG_PARSING_ERROR3
};

struct Scene {
	const stringSet_t nicks {
		"R","A","c","l","ls","ll","sp"
	};
	const floatSet_t softShadowLengths {
		1., 1.1, 1.3, 1.5, 1.7, 1.9, 2.1, 2.3, 2.5, 3., 3.5, 4., 4.5, 5., 5.5, 6., 6.5, 7., 7.5, 8., 8.5, 9., 9.5, 100.0
	};
	const floatSet_t softShadowSoftnesses {
		1.1, 1.3, 1.5, 1.7, 1.9, 2.3, 2.7, 3.1, 3.5, 3.9, 4.3, 4.7, 5.1, 5.5, 5.9, 6.3, 6.7, 7.1, 7.5
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
	std::string header(void);
	void systemDemo(void);
	void mesage(MsgType type, int line = 0, const std::string& hint = "", int error = 0);
	int  saveParsingLog(const char* filename);
	int	 parsing(int ac, char** av);
	int  get_currentCamera(void);
	bool set_currentCamera(int idx);
	int  set_any(std::string string);
	int  set_any(std::istringstream is);
	void set_scenery(A_Scenery* scenery);
	void makeLookatsForCameras(void);
	void rt(void);
	void selectCamera(int ctrl);
	void changeCameraFOV(int ctrl);
	void moveCamera(int ctrl);
	void rotateCamera(int ctrl);
	void flybyCamera(void);
	void changeCamerasOptions(int key, int option);
	float giveValue(const floatSet_t& set, float val, int key);
	friend std::ostream& operator<<(std::ostream& o, const Scene& sc);
};
	int  outputFile(const char* filename);


#endif /* SCENE_HPP */
