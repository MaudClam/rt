//
//  Scene.cpp
//  rt
//
//  Created by uru on 12/07/2024.
//

#include "Scene.hpp"


// Class Scene

Scene::Scene(MlxImage& img) :
img(img),
scenerys(),
objsIdx(),
lightsIdx(),
cameras(),
_resolution(800,600),
_header(),
_ambient(1),
_space(1),
_currentCamera(0) {
	img.set_scene(this);
	_space.invertBrightness();
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
	if (DEBUG_MODE) { std::cout << "~Scene() destructor was done.\n"; }
}

Scene::Scene(const Scene& other) :
img(other.img),
scenerys(other.scenerys),
objsIdx(other.objsIdx),
lightsIdx(other.lightsIdx),
cameras(other.cameras),
_resolution(other._resolution),
_header(other._header),
_ambient(other._ambient),
_space(other._space),
_currentCamera(other._currentCamera)
{}

Scene& Scene::operator=(const Scene& other) {
	if (this != &other) {
		scenerys = other.scenerys;
		objsIdx = other.objsIdx;
		lightsIdx = other.lightsIdx;
		cameras = other.cameras;
		_resolution = other._resolution;
		_header = other._header;
		_ambient = other._ambient;
		_space = other._space;
		_currentCamera = other._currentCamera;
	}
	return *this;
}

int  Scene::parsing(int ac, char** av) {
	(void)ac; (void)av;
	set_any( std::istringstream("R 800 600	RayTrasing") );
	std::string header(_header + " " + std::to_string(_resolution.x) + "x" + std::to_string(_resolution.y));
	img.init(header, _resolution);
	cameras.push_back(Camera(img));// default camera '0'
	
// ============

//	set_any( std::istringstream("A 0.2	255,255,230") );
//	set_any( std::istringstream("c    19,0,19       -1,0,0      70 ") );
//	set_any( std::istringstream("c     0,0,0         0,0,1      70 ") );
//	set_any( std::istringstream("c     0,0,38        0,0,-1     70 ") );
//	set_any( std::istringstream("l     0,0,0      0.6 " + img.white.rrggbb() +  " -1,0,-1") );
//	set_any( std::istringstream("l     0,0,0      0.2 " + img.white.rrggbb() +  " 1,-2,1") );
//	set_any( std::istringstream("sp    0,0,25     16  " + img.white.rrggbb() +  " 1000 0.8") );
//	set_any( std::istringstream("sp    0,0,16.9   8   " + img.yellow.rrggbb() + " 500  0.2") );
//	set_any( std::istringstream("sp    0,0,12.5   4   " + img.cyan.rrggbb() +   " 500  0.3") );
//	set_any( std::istringstream("sp    0,0,9.9    2   " + img.magenta.rrggbb() + " 10  0.4") );
//	set_any( std::istringstream("sp    0,0,3      0.5 " + img.red.rrggbb()) );

// ============

////	set_any( std::istringstream("c     0,0,-12         0,0,1      60 ") );
//	set_any( std::istringstream("c     -40,0,0         1,0,0      60 ") );
////	set_any( std::istringstream("c     0,0,8         0,0,-1      60 ") );
////	set_any( std::istringstream("c     -5,0,3        1,0,0      60 ") );
//	set_any( std::istringstream("c     0,4,0         0,-1,0      60 ") );
//	set_any( std::istringstream("A 0.2	255,255,250") );
//	set_any( std::istringstream("ls    -4,4,8  0.4 " + img.white.rrggbb()) );
//	set_any( std::istringstream("ls     4,4,-8    0.4 " + img.white.rrggbb()) );
//	set_any( std::istringstream("sp    0,-1,0	2   " + img.red.rrggbb()   + " 500  0.2") );
//	set_any( std::istringstream("sp    2,0,0	2   " + img.green.rrggbb()  + " 100 0.3") );
//	set_any( std::istringstream("sp 0,-5001,0 10000 " + img.yellow.rrggbb()+ " 1000 0.5") );
////
//	set_any( std::istringstream("sp    -2,2.5,-4	1   " + img.magenta.rrggbb()   + " -1") );
//	set_any( std::istringstream("sp    -25,0.5,0	9   255,255,0 500   0 0.9 1.33 ") );
//	set_any( std::istringstream("INTERSECTION"));
//	set_any( std::istringstream("sp    -33.5,0.5,0	9   255,255,0 500   0 0.9 1.33 ") );
////	set_any( std::istringstream("END"));
////	set_any( std::istringstream("sp    -33.25,0.5,0	3   255,0,0 500   0 0 1.33 ") );
//	
//	set_any( std::istringstream("sp    -1,1.5,-4	1   " + img.cyan.rrggbb()   + " 10 ") );
//	set_any( std::istringstream("sp    -2,0.1,0	2   255,255,255 500   0 0.99 1.33") );


	set_any( std::istringstream("c     0,1,-25         0,0,1      90 ") );
//	set_any( std::istringstream("c     5,0,3         -1,0,0      60 ") );
//	set_any( std::istringstream("c     0,0,8         0,0,-1      60 ") );
//	set_any( std::istringstream("c     0,0,0        0,0,1      60 ") );
	set_any( std::istringstream("c     0,25,0         0,-1,0      90 ") );
	set_any( std::istringstream("A 0.2	255,255,250") );
	set_any( std::istringstream("ls    25,50,25  0.5 " + img.white.rrggbb()) );
//	set_any( std::istringstream("l    -25,10,25  0.3 " + img.white.rrggbb()) );
//	set_any( std::istringstream("l    -25,15,-25  0.2 " + img.white.rrggbb()) );
	set_any( std::istringstream("l    25,10,-25  0.3 " + img.white.rrggbb()) );
	set_any( std::istringstream("l    0,-5004,250  0.4 " + img.white.rrggbb()) );
	set_any( std::istringstream("l    250,-5004,0  0.4 " + img.white.rrggbb()) );

	set_any( std::istringstream("sp    0,0,0	20   " + img.red.rrggbb()   + " 500  0.2 0 1.33") );
	set_any( std::istringstream("SUBTRACTION"));
	set_any( std::istringstream("sp    0,0,-6	12   " + img.red.rrggbb()   + " -1  0 0 1.33") );
	set_any( std::istringstream("SUBTRACTION"));
	set_any( std::istringstream("sp    6,0,0	12   " + img.red.rrggbb()   + " -1  0 0 1.33") );
	set_any( std::istringstream("SUBTRACTION"));
	set_any( std::istringstream("sp    0,0,6	12   " + img.red.rrggbb()   + " -1  0 0 1.33") );
	set_any( std::istringstream("SUBTRACTION"));
	set_any( std::istringstream("sp    -6,0,0	12   " + img.red.rrggbb()   + " -1  0 0 1.33") );
	set_any( std::istringstream("SUBTRACTION"));
	set_any( std::istringstream("sp    0,0,0	19.5   " + img.red.rrggbb()   + " 500  0 0 1.33") );
	set_any( std::istringstream("UNION"));
	set_any( std::istringstream("sp 0,-5004,0 10000 " + img.yellow.rrggbb()+ " 1000 0.8") );

	set_any( std::istringstream("sp    -3,-3,13	12   " + img.yellow.rrggbb()   + " 500  0.3 0 1.33") );
	set_any( std::istringstream("INTERSECTION"));
	set_any( std::istringstream("sp    3,-3,13	12   " + img.magenta.rrggbb()   + " 500  0.3 0 1.33") );
	set_any( std::istringstream("INTERSECTION"));
	set_any( std::istringstream("sp    0,-3,7	12   " + img.cyan.rrggbb()   + " 500  0.3 0 1.33") );
//	set_any( std::istringstream("UNION"));
//	set_any( std::istringstream("sp 0,-5004,0 10000 " + img.yellow.rrggbb()+ " 1000 0.8") );

	
	
	
//	===========
	
	if (cameras.size() > 1) {
		_currentCamera = 1;
	}
	makeLookatsForCameras();
	if (DEBUG) { std::cout << *this; }
	return SUCCESS;
}

int  Scene::get_currentCamera(void) { return _currentCamera;}

bool Scene::set_currentCamera(int idx) {
	if (idx >= 0 && idx < (int)cameras.size()) {
		_currentCamera = idx;
		Camera* cCam = &cameras[_currentCamera];
		unsigned long size = cameras[_currentCamera].matrix.size();
		unsigned long begin, end;
		std::thread th[NUM_THREADS];
		for (int i = 0; i < NUM_THREADS; i++) {
			begin = i * size / NUM_THREADS;
			if (i == NUM_THREADS - 1) {
				end = size;
			} else {
				end = size / NUM_THREADS * (i + 1);
			}
			th[i] = std::thread([cCam, begin, end](){Camera::restoreRays(cCam, begin, end);});
		}
		for (int i = 0; i < NUM_THREADS; i++) {
			th[i].join();
		}
		if (DEBUG_MODE) { std::cout << "currentCamera: " << _currentCamera << "\n";}
		return true;
	}
	return false;
}

bool Scene::set_any(std::istringstream is) {
	std::string	iD;
	size_t		id = 0;
	
	is >> iD;
	for (; id < nicks.size(); id++) {
		if (nicks[id] == iD)
			break;
	}
	switch (id) {
		case 0: {// R Resolution
			is >> _resolution.x >> _resolution.y >> _header;
			break;
		}
		case 1: {// A AmbientLightning
			is >> _ambient;
//			_space.light = img.white;
			_space = _ambient;
			_space.invertBrightness();
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
				} else if (iD == "XOR") {
					scenerys.back()->combineType = XOR;
				} else if (iD == "END") {
					scenerys.back()->combineType = END;
				} else {
					return false;
				}
			}
		}
	}
	return true;
}

void Scene::set_scenery(A_Scenery* scenery) {
	scenerys.push_back(scenery);
	if ( scenerys.back()->get_isLight() == true ) {
		lightsIdx.push_back(scenery);
	} else {
		objsIdx.push_back(scenery);
	}
}

void Scene::makeLookatsForCameras(void) {
	bool refractionsPresence = false;
	for (auto cam = cameras.begin(), End = cameras.end(); cam != End; ++cam) {
		LookatAux aux(cam->get_pos().n);
		for (auto sc = scenerys.begin(), end = scenerys.end(); sc != end; ++sc) {
			A_Scenery* clone = (*sc)->clone();
			cam->set_scenery(clone);
			if (!refractionsPresence && (*sc)->refractive > 0) {
				refractionsPresence = true;
			}
		}
	}
	for (auto cam = cameras.begin(), End = cameras.end(); cam != End; ++cam) {
		cam->lookatCamera(cam->get_pos());
		cam->initMatrix();
		cam->ambient = _ambient.light;
		cam->space = _space.light;
//		if (refractionsPresence) {
//			cam->recursionDepth *= cam->recursionDepth;
//		}
	}
}

void Scene::rt(void) {
	Camera* cCam = &cameras[_currentCamera];
	cCam->runThreadRoutine(RAY_TRACING);
	cCam->runThreadRoutine(TAKE_PICTURE, &this->img);
	mlx_put_image_to_window(img.get_mlx(), img.get_win(), img.get_image(), 0, 0);
}

void Scene::selectCamera(int ctrl) {
	switch (ctrl) {
		case NEXT: {
			if (set_currentCamera(_currentCamera +  1))
				rt();
			break;
		}
		case PREVIOUS: {
			if (set_currentCamera(_currentCamera -  1))
				rt();
			break;
		}
		default: {
			if (set_currentCamera(ctrl))
				rt();
			break;
		}
	}
	img.flyby = FLYBY_OFF;
}

void Scene::changeCameraFOV(int ctrl) {
	Camera& cam(cameras[_currentCamera]);
	float fovDegree = cam.get_fovDegree();
	switch (ctrl) {
		case INCREASE_FOV: {
			if (cam.resetFovDegree(fovDegree + STEP_FOV)) { rt(); }
			break;
		}
		case DECREASE_FOV: {
			if (cam.resetFovDegree(fovDegree - STEP_FOV)) { rt(); }
			break;
		}
		default:
			break;
		}
}

void Scene::moveCamera(int ctrl) {
	Camera&   cam(cameras[_currentCamera]);
	Position  pos(cam.get_pos());
	switch (ctrl) {
		case MOVE_RIGHT:
			pos.p.x += STEP_MOVE;
			break;
		case MOVE_LEFT:
			pos.p.x -= STEP_MOVE;
			break;
		case MOVE_UP:
			pos.p.y += STEP_MOVE;
			break;
		case MOVE_DOWN:
			pos.p.y -= STEP_MOVE;
			break;
		case MOVE_FORWARD:
			pos.p.z += STEP_MOVE;
			break;
		case MOVE_BACKWARD:
			pos.p.z -= STEP_MOVE;
			break;
		default:
			return;
	}
	cam.lookatCamera(pos);
	rt();
}

void Scene::rotateCamera(int ctrl) {
	Camera&  cam(cameras[_currentCamera]);
	Position pos(cam.get_pos());
	switch (ctrl) {
		case YAW_RIGHT:
			pos.n.turnAroundY(radian(STEP_ROTATION));
			break;
		case YAW_LEFT:
			pos.n.turnAroundY(radian(-STEP_ROTATION));
			break;
		case PITCH_UP:
			pos.n.turnAroundX(radian(-STEP_ROTATION));
			break;
		case PITCH_DOWN:
			pos.n.turnAroundX(radian(STEP_ROTATION));
			break;
		case ROLL_RIGHT: {
			cam.resetRoll(cam.get_rollDegree() + STEP_ROTATION);
			rt();
			return;
		}
		case ROLL_LEFT:
			cam.resetRoll(cam.get_rollDegree() - STEP_ROTATION);
			rt();
			return;
		default:
			return;
	}
	cam.lookatCamera(pos);
	rt();
}

void Scene::flybyCamera(void) {
	Camera&		cam(cameras[_currentCamera]);
	Position	pos(cam.get_pos());
	float angle = radian(FLYBY_STEP / 10.), radius = cam.get_flybyRadius();
	if (img.flyby == FLYBY_CLOCKWISE) {
		angle = -angle;
	}
	pos.p.z += radius;
	pos.p.turnAroundY(angle);
	pos.n.turnAroundY(-angle).normalize();
	pos.p.z -= radius;
	cam.lookatCamera(pos);
	rt();
}


// Non member functions

std::ostream& operator<<(std::ostream& o, Scene& sc) {
	std::ostringstream os;
	o  << "R " << std::setw(5) << sc._resolution << " " << sc._header << std::endl;
	os << "A  " << std::setw(32) << sc._ambient;
	o << std::setw(56) << std::left << os.str();
	o << " #ambient liting" << std::endl;
	for (auto light = sc.lightsIdx.begin(); light != sc.lightsIdx.end(); ++light) {
		o << *(*light) << std::endl;
	}
	int i = 0;
	for (auto camera = sc.cameras.begin(); camera != sc.cameras.end(); ++camera, ++i) {
		o << *camera << " " << i << std::endl;
	}
	for (auto obj = sc.objsIdx.begin(); obj != sc.objsIdx.end(); ++obj) {
		o << *(*obj) << std::endl;
	}
	return o;
}
