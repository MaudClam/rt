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
typedef std::vector<Camera>	cameras_t;

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
	friend std::ostream& operator<<(std::ostream& o, Scene& sc);
};
	int  outputFile(const char* filename);


#endif /* SCENE_HPP */
