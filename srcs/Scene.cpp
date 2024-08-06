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
	set_any( std::istringstream("R 600 600	RayTrasing") );
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

	set_any( std::istringstream("c     0,0,0         0,0,1      60 ") );
	set_any( std::istringstream("c     5,0,3         -1,0,0      60 ") );
	set_any( std::istringstream("c     0,0,8         0,0,-1      60 ") );
	set_any( std::istringstream("c     -5,0,3        1,0,0      60 ") );
	set_any( std::istringstream("c     0,4,2         0,-1,0      60 ") );
	set_any( std::istringstream("A 0.2	255,255,250") );
	set_any( std::istringstream("l     2,1,0    0.6 " + img.white.rrggbb()) );
	set_any( std::istringstream("ls     1,4,4  0.2 " + img.white.rrggbb()) );
	set_any( std::istringstream("sp    0,-1,3	2   " + img.red.rrggbb()   + " 500  0.2") );
	set_any( std::istringstream("sp    2,0,4	2   " + img.blue.rrggbb()  + " 500  0.3") );
	set_any( std::istringstream("sp    -2,0,4	2   " + img.green.rrggbb() + " 10   0.4") );
	set_any( std::istringstream("sp 0,-5001,0 10000 " + img.yellow.rrggbb()+ " 1000 0.5") );

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
		cameras[_currentCamera].restoreRays();
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
		default:
			return false;
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

void Scene::indexingScenerys(void) {
	objsIdx.clear();
	lightsIdx.clear();
	auto End = scenerys.end();
	for (auto scenery = scenerys.begin(); scenery != End; ++scenery) {
		if ( (*scenery)->get_isLight() ) {
			lightsIdx.push_back(*scenery);
		} else {
			objsIdx.push_back(*scenery);
		}
	}
}

void Scene::makeLookatsForCameras(void) {
	for (auto cam = cameras.begin(), End = cameras.end(); cam != End; ++cam) {
		LookatAux aux(cam->get_pos().n);
		for (auto sc = scenerys.begin(), end = scenerys.end(); sc != end; ++sc) {
			A_Scenery* clone = (*sc)->clone();
			cam->set_scenery(clone);
		}
	}
	for (auto cam = cameras.begin(), End = cameras.end(); cam != End; ++cam) {
		cam->lookatCamera(cam->get_pos());
		cam->initMatrix();
	}
}

void Scene::raytrasing(void) {
	Camera&	cam(cameras[_currentCamera]);
	for (auto pixel = cam.matrix.begin(), End = cam.matrix.end(); pixel != End; ++pixel) {
		for (auto ray = pixel->rays.begin(), end = pixel->rays.end(); ray != end; ++ray) {
			trasingRay(*ray, 0);
		}
		pixel->averageColor();
	}
}

A_Scenery* Scene::nearestIntersection(Ray& ray, bool notOptimize) {
	A_Scenery*	nearestObj = NULL;
	float		distance = INFINITY;
	Camera&		cam(cameras[_currentCamera]);
	for (auto obj = cam.scenerys.begin(), end = cam.scenerys.end(); obj != end; ++obj) {
		if ( (*obj)->intersection(ray, notOptimize) ) {
			if (distance > ray.dist) {
				distance = ray.dist;
				nearestObj = *obj;
			}
		}
	}
	if (nearestObj) {
		ray.dist = distance;
	}
	return nearestObj;
}

bool Scene::shadow(Ray& ray) {
	float distance = ray.dist;
	ray.movePovByEpsilon();
	Camera& cam(cameras[_currentCamera]);
	for (auto obj = cam.scenerys.begin(), end = cam.scenerys.end(); obj != end; ++obj) {
		if ( (*obj)->intersection(ray) ) {
			if (ray.dist < distance) {
				return true;
			}
		}
	}
	return false;
}

void Scene::trasingRay(Ray& ray, int recursion) {
	if (recursion <= RECURSIONS) {
		A_Scenery* obj = nearestIntersection(ray, recursion);
		if (obj) {
			if (recursion == 0) {
				ray.camDir = ray.dir;
			}
			ray.collectLight(obj->color,_ambient.light);
			ray.changePov();
			obj->getNormal(ray);
			Ray reflect;
			reflect.reflect(ray);
			Camera& cam(cameras[_currentCamera]);
			for (auto light = cam.lightsIdx.begin(), end = cam.lightsIdx.end(); light != end; ++light) {
				if ( (*light)->lighting(ray) && !shadow(ray) ) {
					ray.collectLight(obj->color);
					ray.collectSpecularLight(obj->color, obj->specular);
				}
			}
			if (obj->reflective > 0) {
				trasingRay(reflect, ++recursion);
				ray.collectReflectiveLight(reflect.color, obj->reflective);
			}
		} else {
			ray.color = _space.light;
		}
	}
}

void Scene::rt(void) {
	raytrasing();
	cameras[_currentCamera].takePicture(img);
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
			if (cam.reset_fovDegree(fovDegree + STEP_FOV)) { rt(); }
			break;
		}
		case DECREASE_FOV: {
			if (cam.reset_fovDegree(fovDegree - STEP_FOV)) { rt(); }
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
			break;
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
		case ROLL_RIGHT:
			pos.n.turnAroundZ(radian(STEP_ROTATION));
			break;
		case ROLL_LEFT:
			pos.n.turnAroundZ(radian(-STEP_ROTATION));
			break;
		default:
			break;
	}
	cam.lookatCamera(pos);
	rt();
}

void Scene::calculateFlybyRadius(void) {
//	Camera&		cam(cameras[_currentCamera]);
//	Position	pos = cam.get_pos();
////	float		tan = cam.get_fovTan();
//	float		back = 0;
//	float		front = INFINITY;
//
//	auto End = cam.matrix.end();
//	for (auto pixel = cam.matrix.begin(); pixel != End; ++pixel) {
//		auto end = objsIdx.end();
//		for (auto obj = objsIdx.begin(); obj != end; ++obj) {
//			if ( (*obj)->intersection(pixel->ray, FRONT) ) {
//				if ( front > pixel->ray.dist ) {
//					front = pixel->ray.dist;
//				}
//			}
////			pixel->reset(tan, pos);
//			if ( (*obj)->intersection(pixel->ray, BACK) ) {
//				if ( back < pixel->ray.dist && pixel->ray.dist < FLYBY_RADIUS_MAX) {
//					back = pixel->ray.dist;
//				}
//			}
//		}
//	}
//	if ( back > 0) {
//		float r = (back - front) / 2 + front;
//		cam.set_flybyRadius(r);
//		if (DEBUG_MODE) {
//			std::cout << "front: " << front << ", back: " << back;
//			std::cout << ", flybyRadius: " << r << std::endl;
//		}
//	}
}

void Scene::flybyCamera(void) {
	Camera& cam(cameras[_currentCamera]);
	float angle = radian(FLYBY_STEP / 10.), radius = cam.get_flybyRadius();
	if (img.flyby == FLYBY_CLOCKWISE) {
		angle = -angle;
	}
	Position pos(cam.get_pos());
	pos.p.z += radius;
	pos.p.turnAroundY(angle);
	pos.n.turnAroundY(-angle).normalize();
	pos.p.z -= radius;
	rt();
}


// Non member functions

std::ostream& operator<<(std::ostream& o, Scene& sc) {
	std::ostringstream os;
	o  << "R " << std::setw(5) << sc._resolution << " " << sc._header << std::endl;
	os << "A  " << std::setw(32) << sc._ambient;
	o << std::setw(46) << std::left << os.str();
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
