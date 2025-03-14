#include "Scene.hpp"


// struct Cameras

Cameras::Cameras(void) : std::vector<Camera>() {}
Cameras::~Cameras(void) {}
Cameras& Cameras::clear_(int n) {
	Cameras tmp;
	if (n) tmp.reserve(n);
	swap(tmp);
	return *this;
}


// Class Scene

Scene::Scene(MlxImage& img) :
img(img),
scenerys(),
objsIdx(),
lightsIdx(),
cameras(),
phMap(),
_resolution(DEFAULT_RESOLUTION),
_header(),
_ambient(1),
_background(1),
_currentCamera(0) {
	img.set_scene(this);
	_background.set_ratio(_ambient.get_ratio() * BACKGRND_VISIBILITY_FACTOR);
//	_background.invertBrightness();
}

Scene::~Scene(void) {
	for (auto scenery = scenerys.begin(), end = scenerys.end(); scenery != end; ++scenery) {
		delete *scenery;
		*scenery = NULL;
	}
	for (auto cam = cameras.begin(), End = cameras.end(); cam != End; ++cam) {
		for (auto scenery = cam->scenerys.begin(), end = cam->scenerys.end(); scenery != end; ++scenery) {
			delete *scenery;
			*scenery = NULL;
		}
	}
}

Scene::Scene(const Scene& other) :
img(other.img),
scenerys(other.scenerys),
objsIdx(other.objsIdx),
lightsIdx(other.lightsIdx),
cameras(other.cameras),
phMap(other.phMap),
_resolution(other._resolution),
_header(other._header),
_ambient(other._ambient),
_background(other._background),
_currentCamera(other._currentCamera)
{}

Scene& Scene::operator=(const Scene& other) {
	if (this != &other) {
		scenerys = other.scenerys;
		objsIdx = other.objsIdx;
		lightsIdx = other.lightsIdx;
		cameras = other.cameras;
		phMap = other.phMap;
		_resolution = other._resolution;
		_header = other._header;
		_ambient = other._ambient;
		_background = other._background;
		_currentCamera = other._currentCamera;
	}
	return *this;
}

std::string Scene::header(void) {
	return std::string(_header + " " + std::to_string(_resolution.x) + "x" + std::to_string(_resolution.y));
}

void Scene::systemDemo(void) {
	set_any("R	800 600  SystemDemo");
	img.init(header(), _resolution);
	cameras.push_back(Camera(img));
	set_any("A				0.25		0xFFFFEE");
	set_any("l	2,1,0		0.5		0xFFFFFF");
	set_any("l	1,4,4		0.3		0xFFFFFF");
//	set_any("ll	1,4,4		0.3		0xFFFFFF	10,40,40");
	set_any("c	0,0,-2.5	0,0,1		60");
	set_any("c	0,0,8		0,0,-1		60");
	set_any("c	-5,0,3		1,0,0		60");
	set_any("c	5,0,3		-1,0,0		60");
	set_any("c	0,5,3		0,-1,0		60");
	set_any("sp	0,-1,3		2		0xFF0000	500		0.1");
	set_any("sp	2,0,4		2		0xFFFFFF	500		0.0		1.0		1.5");
	set_any("sp	0,0.25,5.5		2.5		0xF0F0F0	500		1.0");
	set_any("sp	-2,0,4		2		0x00FF00	0.2		0.2");
	set_any("sp	0,-5001,0	10000	0xFFFF00	1000	0.3");
	if (cameras.size() > 1)
		_currentCamera = 1;
	saveParsingLog(PARSING_LOGFILE);
	phMap.make(scenerys, lightsIdx);
	makeLookatsForCameras();
	cameras[_currentCamera].calculateFlybyRadius();
//	img.flyby = COUNTER_CLOCKWISE;
}

void Scene::mesage(MsgType type, int line, const std::string& hint, int error) {
	switch (type) {
		case WELLCOM_MSG:
			std::cout << "\nWellcom to Ray Tracing by " << CREATORS << "!" <<std::endl;
			break;
		case WRNG_FILE_MISSING:
			std::cerr << "\nWarning: Missing *.rt file. System demo will be used." <<std::endl;
			std::cerr << "How to create an *.rt file see default.rt." <<std::endl;
			std::cerr << "Usage:\n\t\t./rt <filename.rt>" <<std::endl;
			break;
		case ERR_OPEN_FILE:
			std::cerr << "\nError: Can't open file: '";
			std::cerr << hint << "'. " << strerror(error) << std::endl;
			break;
		case WRNG_RESOLUTION:
			std::cerr << "\nWarning: Resolution is missing in the first line of the file: '" << hint << "'." << std::endl;
			std::cerr << "Will be used by default: '" << _resolution.x << "x" << _resolution.y << "'." << std::endl;
			std::cerr << "See " << PARSING_LOGFILE << " for datails." << std::endl;
			break;
		case WRNG_PARSING_ERROR:
			std::cerr << "\nWarning: Parsing error in file: '" << hint << "'." << std::endl;
			std::cerr << "Line " << line << ": unknown object, ignored." << std::endl;
			std::cerr << "See " << PARSING_LOGFILE << " for datails." << std::endl;
			break;
		case WRNG_PARSING_ERROR1: {
			std::cerr << "\nWarning: Parsing error in file: '" << hint << "'." << std::endl;
			std::cerr << "The scene does not contain any cameras." << std::endl;
			Position pos(cameras.front().get_pos());
			float degree = cameras.front().get_fov().get_degree();
			std::cerr << "Will be used the system camera at position " << pos << " with FOV " << degree << "º." << std::endl;
			std::cerr << "See " << PARSING_LOGFILE << " for datails." << std::endl;
			break;
		}
		case WRNG_PARSING_ERROR2:
			std::cerr << "\nWarning: Parsing error in file: '" << hint << "'." << std::endl;
			std::cerr << "The scene does not contain any light sources." << std::endl;
			std::cerr << "See " << PARSING_LOGFILE << " for datails." << std::endl;
			break;
		case WRNG_PARSING_ERROR3:
			std::cerr << "\nWarning: Parsing error in file: '" << hint << "'." << std::endl;
			std::cerr << "The scene does not contain any objects." << std::endl;
			std::cerr << "See " << PARSING_LOGFILE << " for datails." << std::endl;
			break;
		default:
			break;
	}
}

int Scene::saveParsingLog(const char* filename) {
	std::string		fileName(filename);
	std::fstream	out;
	if (DEBUG_MODE) fileName = DEBUG_PATH + fileName;
	out.open(fileName, std::ios::out | std::ios::trunc);
	if (out.fail()) {
		int error = errno;
		std::cerr	<< "\nWarning: Can't open file: "
					<< "'" << fileName << "'. "
					<< strerror(error) << std::endl;
		return error;
	}
	out << *this;
	out.close();
	return SUCCESS;
}

int  Scene::parsing(int ac, char** av) {
	if (ac != 2) {
		mesage(WRNG_FILE_MISSING);
		systemDemo();
		return SUCCESS;
	}
	std::ifstream	in;
	std::string		line;
	in.open(av[1],std::ifstream::in);
	if (in.fail()) {
		int error = errno;
		mesage(ERR_OPEN_FILE, 0, av[1], error);
		return error;
	}
	std::getline(in, line);
	int res = set_any(line);
	if (res == ERROR) {
		mesage(WRNG_PARSING_ERROR, 1, av[1]);
	} else if (res > 0) {
		mesage(WRNG_RESOLUTION, 1, av[1]);
		_resolution.set_xy(DEFAULT_RESOLUTION);
		_header = "Default";
	}
	img.init(header(), _resolution);
	cameras.push_back(Camera(img));	// default camera '0'
	for (int ln = 2; !in.eof(); ln++) {
		std::getline(in, line);
		if ( line.compare(0, 1, "#") && !line.empty() ) {
			if (set_any(line) == ERROR) {
				mesage(WRNG_PARSING_ERROR, ln, av[1]);
			}
		}
	}
	in.close();
	if (cameras.size() > 1) {
		_currentCamera = 1;
	} else {
		mesage(WRNG_PARSING_ERROR1, 0, av[1]);
	}
	if (lightsIdx.empty()) {
		mesage(WRNG_PARSING_ERROR2, 0, av[1]);
	}
	if (objsIdx.empty()) {
		mesage(WRNG_PARSING_ERROR3, 0, av[1]);
	}
	saveParsingLog(PARSING_LOGFILE);
	phMap.make(scenerys, lightsIdx);
	makeLookatsForCameras();
	return SUCCESS;
}

int  Scene::get_currentCamera(void) { return _currentCamera;}

bool Scene::set_currentCamera(int idx) {
	if (idx >= 0 && idx < (int)cameras.size() && _currentCamera != idx) {
		_currentCamera = idx;
		Camera* cCam = &cameras[_currentCamera];
		cCam->runThreadRoutine(RESTORE_RAYS);
		if (DEBUG_KEYS) { std::cout << "currentCamera: " << _currentCamera << "\n";}
		return true;
	}
	return false;
}

int Scene::set_any(std::string string) {
	return set_any(std::istringstream(string));
}

int Scene::set_any(std::istringstream is) {
	std::string	iD;
	std::string phMapType;
	size_t		id = 0;

	is >> iD;
	for (; id < nicks.size(); id++) {
		if (nicks[id] == iD)
			break;
	}
	switch (id) {
		case 0: {// R Resolution
			is >> _resolution.x >> _resolution.y >> _header >> phMapType >> phMap.totalPhotons >> phMap.estimate >> phMap.gridStep ;
			_resolution.x = i2limits(_resolution.x, RESOLUTION_MIN, RESOLUTION_MAX);
			_resolution.y = i2limits(_resolution.y, RESOLUTION_MIN, RESOLUTION_MAX);
			phMap.totalPhotons = i2limits(phMap.totalPhotons, 0, MAX_PHOTONS_NUMBER);
			phMap.estimate = i2limits(phMap.estimate, 0, MAX_ESTIMATE_PHOTONS);
			phMap.gridStep = f2limits(phMap.gridStep, 0.01, MAX_PHOTONS_GRID_STEP);
			if (phMapType == "GLOBAL") {
				phMap.type = GLOBAL;
			} else if (phMapType == "CAUSTIC") {
				phMap.type = CAUSTIC;
			} else if (phMapType == "VOLUME") {
				phMap.type = VOLUME;
			} else if (phMapType == "NO") {
				phMap.type = NO;
				phMap.totalPhotons = phMap.estimate = phMap.gridStep = 0;
			} else {
				phMap.type = NO;
				if (phMapType != "")
					return ERROR;
			}
			break;
		}
		case 1: {// A AmbientLightning
			is >> _ambient;
			_background = _ambient;
			_background.set_ratio(_ambient.get_ratio() * BACKGRND_VISIBILITY_FACTOR);
//			_background.invertBrightness();
			break;
		}
		case 2: {// c camera
			cameras.push_back(Camera(img));
			is >> cameras.back();
			break;
		}
		case 3: {// l spotlight
			Light* l = new Light();
			l->set_nick(nicks[id]);
			l->set_name("spotlight");
			l->set_type(Light::SPOTLIGHT);
			is >> *l;
			set_scenery(l);
			break;
		}
		case 4: {// ls sunlitght
			Light* l = new Light();
			l->set_nick(nicks[id]);
			l->set_name("sunlight");
			l->set_type(Light::SUNLIGHT);
			is >> *l;
			set_scenery(l);
			break;
		}
		case 5: {// ll sunlight limited by plane
			Light* l = new Light();
			l->set_nick(nicks[id]);
			l->set_name("sunlight limited by plane");
			l->set_type(Light::SUNLIGHT_LIMITED);
			is >> *l;
			set_scenery(l);
			break;
		}
		case 6: {// sp sphere
			Sphere* sp = new Sphere;
			is >> *sp;
			set_scenery(sp);
			break;
		}
		default: {//
			if (scenerys.size() > 0) {
				if (iD == "UNION") {
					scenerys.back()->combineType = UNION;
				} else if (iD == "SUBTRACTION") {
					scenerys.back()->combineType = SUBTRACTION;
				} else if (iD == "INTERSECTION") {
					scenerys.back()->combineType = INTERSECTION;
				} else if (iD == "END") {
					scenerys.back()->combineType = END;
				} else {
					return ERROR;
				}
			}
		}
	}
	return (int)id;
}

void Scene::set_scenery(A_Scenery* scenery) {
	scenerys.push_back(scenery);
	scenerys.back()->set_id((int)scenerys.size());
	if ( scenerys.back()->get_isLight() ) {
		lightsIdx.push_back(scenery);
		scenery->set_color(scenery->light.light);
	} else {
		objsIdx.push_back(scenery);
	}
}

void Scene::makeLookatsForCameras(void) {
	for (auto cam = cameras.begin(), End = cameras.end(); cam != End; ++cam) {
		for (auto sc = scenerys.begin(), end = scenerys.end(); sc != end; ++sc) {
			A_Scenery* clone = (*sc)->clone();
			cam->set_scenery(clone);
		}
		if (phMap.type != NO)
			cam->phMap = phMap;
	}
	for (auto cam = cameras.begin(), End = cameras.end(); cam != End; ++cam) {
		cam->lookatCamera(cam->get_pos());
		cam->initMatrix();
		cam->ambient = _ambient;
		cam->background = _background;
	}
}

void Scene::rt(void) {
//	if (DEBUG_MODE) std::cout << "Tracing..." << std::endl;
	Camera* cCam = &cameras[_currentCamera];
	cCam->runThreadRoutine(RAY_TRACING);
	cCam->runThreadRoutine(TAKE_PICTURE, &this->img);
	mlx_put_image_to_window(img.get_mlx(), img.get_win(), img.get_image(), 0, 0);
}

void Scene::selectCamera(int ctrl) {
	if (cameras[_currentCamera].tracingType == RAY) {
		switch (ctrl) {
			case NEXT:		if (!set_currentCamera(_currentCamera +  1)) return;
				break;
			case PREVIOUS:	if (!set_currentCamera(_currentCamera -  1)) return;
				break;
			default:		if (!set_currentCamera(ctrl)) return;
				break;
		}
		img.flyby = OFF;
		rt();
	}
}

void Scene::changeCameraFOV(int ctrl) {
	if (cameras[_currentCamera].tracingType == RAY) {
		Camera& cam(cameras[_currentCamera]);
		float fovDegree = cam.get_fovDegree();
		switch (ctrl) {
			case INCREASE:	if (!cam.resetFovDegree(fovDegree + (float)STEP_FOV)) return;
				break;
			case DECREASE:	if (!cam.resetFovDegree(fovDegree - (float)STEP_FOV)) return;
				break;
			default:		if (!cam.resetFovDegree((float)ctrl)) return;
				break;
		}
		rt();
	}
}

void Scene::moveCamera(int ctrl) {
	if (cameras[_currentCamera].tracingType == RAY) {
		Camera&   cam(cameras[_currentCamera]);
		Position  pos(cam.get_pos());
		switch (ctrl) {
			case RIGHT:		pos.p.x += (float)STEP_MOVE; break;
			case LEFT:		pos.p.x -= (float)STEP_MOVE; break;
			case UP:		pos.p.y += (float)STEP_MOVE; break;
			case DOWN:		pos.p.y -= (float)STEP_MOVE; break;
			case FORWARD:	pos.p.z += (float)STEP_MOVE; break;
			case BACKWARD:	pos.p.z -= (float)STEP_MOVE; break;
			default:		return;
		}
		cam.lookatCamera(pos);
		rt();
	}
}

void Scene::rotateCamera(int ctrl) {
	if (cameras[_currentCamera].tracingType == RAY) {
		Camera&  cam(cameras[_currentCamera]);
		Position pos(cam.get_pos());
		switch (ctrl) {
			case YAW_RIGHT:		pos.n.turnAroundY(degree2radian((float)STEP_ROTATION)); break;
			case YAW_LEFT:		pos.n.turnAroundY(degree2radian(-(float)STEP_ROTATION)); break;
			case PITCH_UP:		pos.n.turnAroundX(degree2radian((float)STEP_ROTATION)); break;
			case PITCH_DOWN:	pos.n.turnAroundX(degree2radian(-(float)STEP_ROTATION)); break;
			case ROLL_RIGHT:	cam.resetRoll(cam.get_rollDegree() + (float)STEP_ROTATION); rt(); return;
			case ROLL_LEFT:		cam.resetRoll(cam.get_rollDegree() - (float)STEP_ROTATION); rt(); return;
			default:			return;
		}
		cam.lookatCamera(pos);
		rt();
	}
}

void Scene::flybyCamera(void) {
	if (cameras[_currentCamera].tracingType == RAY &&
		cameras[_currentCamera].photonMap == NO) {
		Camera&		cam(cameras[_currentCamera]);
		Position	pos(cam.get_pos());
		float angle = degree2radian(FLYBY_STEP / 10.), radius = cam.get_flybyRadius();
		if (img.flyby == CLOCKWISE) {
			angle = -angle;
		}
		pos.p.z += radius;
		pos.p.turnAroundY(angle);
		pos.n.turnAroundY(-angle).normalize();
		pos.p.z -= radius;
		cam.lookatCamera(pos);
		rt();
	} else {
		img.flyby = OFF;
	}
}

void Scene::changeCamerasOptions(int key, int option) {
	if (cameras.empty()) return;
	switch (option) {
		case SMOOTHING_FACTOR: {
			if (key == 0 || key > 4 || key == cameras[0].get_sm()) return;
			break;
		}
		case RECURSION_DEPTH: {
			if (key == cameras[0].depth) return;
			break;
		}
		case PATHS_PER_RAY: {
			if (cameras[0].tracingType != PATH) return;
			if (key == 0 || key > 4 || pprs[key] == cameras[0].paths) return;
			img.flyby = OFF;
			break;
		}
		case PHOTON_MAP: {
			if (cameras[0].phMap.type == NO) return;
//			if (cameras[0].tracingType == PATH) return;
			if (cameras[0].photonMap == NO && (MapType)key == NO) return;
			img.flyby = OFF;
			break;
		}
		case OTHER: {
			switch (key) {
				case RAYTRACING: { if (cameras[0].tracingType == RAY) return;
					img.flyby = OFF;
					break;
				}
				case PATHTRACING: { if (cameras[0].tracingType == PATH) return;
					img.flyby = OFF;
					break;
				}
				default:
					break;
			}
		}
		default:
			break;
	}
	for (auto cam = cameras.begin(), end = cameras.end(); cam != end; ++cam) {
		switch (option) {
			case SMOOTHING_FACTOR:	cam->resetSmoothingFactor(key); break;
			case RECURSION_DEPTH:	cam->resetRecursionDepth(key); break;
			case PATHS_PER_RAY:		cam->resetPathsPerRay(key); break;
			case PHOTON_MAP:		cam->changePhotonMap((MapType)key); break;
			case OTHER:				cam->changeOther((Controls)key); break;
			default: return;
		}
	}
	rt();
}


// Non member functions

std::ostream& operator<<(std::ostream& o, const Scene& sc) {
	std::ostringstream os;
	o	<< "R " << std::setw(5) << sc._resolution.x << " " << sc._resolution.y << " " << sc._header;
	if (sc.phMap.type != NO) {
		o	<< " " << mapType(sc.phMap.type) << " " << sc.phMap.totalPhotons << " "
			<< sc.phMap.estimate << " " << sc.phMap.gridStep;
	}
	o	<< std::endl;
	os	<< "A  " << std::setw(32) << sc._ambient;
	o	<< std::setw(56) << std::left << os.str();
	o	<< " #ambient liting" << std::endl;
	for (auto light = sc.lightsIdx.begin(); light != sc.lightsIdx.end(); ++light) {
		o << *(*light) << std::endl;
	}
	int i = 0;
	for (auto camera = ++sc.cameras.begin(); camera != sc.cameras.end(); ++camera, ++i) {
		o << *camera << " " << i << std::endl;
	}
	for (auto obj = sc.objsIdx.begin(); obj != sc.objsIdx.end(); ++obj) {
		o << *(*obj) << std::endl;
		if ((*obj)->combineType) {
			o << combineType((*obj)->combineType) << std::endl;
		}
	}
	return o;
}

int outputFile(const char* filename) {
	std::string		fileName(filename);
	std::ifstream	in;
	std::string		line;
	if (DEBUG_MODE) fileName = DEBUG_PATH + fileName;
	in.open(fileName, std::ifstream::in);
	if (in.fail()) {
		int error = errno;
		std::cerr	<< "\nWarning: Can't open file: "
					<< "'" << fileName << "'. "
					<< strerror(error) << std::endl;
		return error;
	}
	while (!in.eof()) {
		std::getline(in, line);
		std::cout << line << "\n";
	}
	in.close();
	return SUCCESS;
}
