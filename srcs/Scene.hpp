#ifndef SCENE_HPP
# define SCENE_HPP

# include <fstream>
# include <random>
# include "MlxImage.hpp"
# include "camera.hpp"
# include "PhotonMap.hpp"
# include "Sphere.hpp"
# include "Light.hpp"

class	MlxImage;
class	Camera;

typedef std::vector<std::string>				stringSet_t;
typedef std::vector<float>						floatSet_t;
typedef	std::random_device						rand_device_t;
typedef	std::mt19937							rand_gen_t;
typedef	std::uniform_real_distribution<float>	rand_distr_t;

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


struct Cameras : public std::vector<Camera> {
	Cameras(void);
	~Cameras(void);
	Cameras& clear_(int n);
};


struct Scene {
	const stringSet_t nicks {
		"R","A","c","l","ls","ll","sp"
	};
	MlxImage&	img;
	Scenerys	scenerys;
	Scenerys	objsIdx;
	Scenerys	lightsIdx;
	Cameras		cameras;
	PhotonMap	phMap;
private:
	Vec2i		_resolution;
	std::string	_header;
	Lighting	_ambient;
	Lighting	_background;
	int			_currentCamera;
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
	friend std::ostream& operator<<(std::ostream& o, const Scene& sc);
};
	int  outputFile(const char* filename);

#endif /* SCENE_HPP */
