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
_base(BASE),
_resolution(800,600),
_header(),
_ambient(1),
_space(1),
_currentCamera(0) {
	img.set_scene(this);
	_space.invertBrightness();
}

Scene::~Scene(void) {
	auto End = scenerys.end();
	for (auto scenery = scenerys.begin(); scenery != End; ++scenery) {
		delete *scenery;
		*scenery = NULL;
	}
	if (DEBUG_MODE) { std::cout << "~Scene() destructor was done.\n"; }
}

Scene::Scene(const Scene& other) :
img(other.img),
scenerys(other.scenerys),
objsIdx(other.objsIdx),
lightsIdx(other.lightsIdx),
cameras(other.cameras),
_base(other._base),
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

int  Scene::get_currentCamera(void) { return _currentCamera;}

bool Scene::set_currentCamera(int cameraIdx) {
	if (checkCameraIdx(cameraIdx)) {
		this->_currentCamera = cameraIdx;
		cameras[_currentCamera].resetMatrix();
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
	switch (id) {// bc BasicCoordinate
		case 0: {
			is >> _base.p >> _base.n;
			_base.n.normalize();
			break;
		}
		case 1: {// R Resolution
			is >> _resolution.x >> _resolution.y >> _header;
			break;
		}
		case 2: {// A AmbientLightning
			is >> _ambient;
			_space = _ambient;
			_space.invertBrightness();
			break;
		}
		case 3: {// c camera
			cameras.push_back(Camera(img, _base));
			is >> cameras.back();
			break;
		}
		case 4: {// c litght
			Light* l = new Light();
			is >> *l;
			scenerys.push_back(l);
			break;
		}
		case 5: {// sp sphere
			Sphere* sp = new Sphere;
			is >> *sp;
			scenerys.push_back(sp);
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

int  Scene::parsing(int ac, char** av) {
	(void)ac; (void)av;
	set_any( std::istringstream("R 800 600	RayTrasing") );
	std::string header(_header + " " + std::to_string(_resolution.x) + "x" + std::to_string(_resolution.y));
	img.init(header, _resolution);
//	set_any( std::istringstream("A 0.2	255,255,230") );
//	set_any( std::istringstream("c    19,0,19       -1,0,0      70 ") );
//	set_any( std::istringstream("c     0,0,0         0,0,1      70 ") );
//	set_any( std::istringstream("c     0,0,38        0,0,-1     70 ") );
//	set_any( std::istringstream("l     0,0,0      0.6 " + img.white.rrggbb() + " -1,0,-1") );
//	set_any( std::istringstream("l     0,0,0      0.2 " + img.white.rrggbb() + " 1,-2,1") );
//	set_any( std::istringstream("sp    0,0,25     16  " + img.white.rrggbb()) );
//	set_any( std::istringstream("sp    0,0,16.9   8   " + img.yellow.rrggbb()) );
//	set_any( std::istringstream("sp    0,0,12.5   4   " + img.cyan.rrggbb()) );
//	set_any( std::istringstream("sp    0,0,9.9    2   " + img.magenta.rrggbb()) );
//	set_any( std::istringstream("sp    0,0,3      0.5 " + img.red.rrggbb()) );

// ============

	set_any( std::istringstream("c     0,0,0         0,0,1      60 ") );
	set_any( std::istringstream("A 0.2	255,255,250") );
	set_any( std::istringstream("l     2,1,0    0.6 " + img.white.rrggbb() + " 0,0,0") );
	set_any( std::istringstream("l     0,0,0    0.2 " + img.white.rrggbb() + " 1,4,4") );
	set_any( std::istringstream("sp    0,-1,3	2 " + img.red.rrggbb()) );
	set_any( std::istringstream("sp    2,0,4	2 " + img.blue.rrggbb()) );
	set_any( std::istringstream("sp    -2,0,4	2 " + img.green.rrggbb()) );
	set_any( std::istringstream("sp    0,-5001,0	10000 " + img.yellow.rrggbb()) );

//	===========
	initLoockats();
	if (DEBUG) { std::cout << *this; }
	return SUCCESS;
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

void Scene::initLoockats(void) {
	BasicCoordinate* bc = new BasicCoordinate(_base);
	scenerys.insert(scenerys.begin(), bc);			// BasicCoordinate
	cameras.insert(cameras.begin(), Camera(img, _base));	// default camera "0"
	indexingScenerys();
	auto End = cameras.end();
	for (auto camera = cameras.begin(); camera != End; ++camera) {
		Position eye(camera->get_pos());
		if (_base.n != eye.n) {
			LookatAux aux(eye.n);
			auto end = scenerys.end();
			for (auto scenery = scenerys.begin(); scenery != end; ++scenery ) {
				(*scenery)->set_lookatCamera(eye, aux);
			}
			camera->reset_pov(_base);
		} else {
			LookatAux aux(eye.n);
			auto end = scenerys.end();
			for (auto scenery = scenerys.begin(); scenery != end; ++scenery ) {
				(*scenery)->set_lookatBase();
			}
			camera->reset_pov(eye);
		}
	}
	if (cameras.size() > 1) {
		_currentCamera = 1;
	}
}

bool Scene::checkCameraIdx(int cameraIdx) const {
	if (cameraIdx >= 0 && cameraIdx < (int)cameras.size()) {
		return true;
	}
	if (DEBUG_MODE) {
		std::cerr	<< "CameraIdx '" << cameraIdx << "' is out of range, "
		<< "saved current camera '" << _currentCamera << "'" << std::endl;
	}
	return false;
}

void Scene::recalculateLookatsForCurrentCamera(const Position& eye) {
	Camera&	cam(cameras[_currentCamera]);
	float	roll = cam.get_roll();
	auto	End = scenerys.end();
	if (_base.n != eye.n) {
		LookatAux aux(eye.n);
		for (auto scenery = scenerys.begin(); scenery != End; ++scenery) {
			(*scenery)->recalculateLookat(_currentCamera, eye, aux);
			(*scenery)->recalculateLookat(_currentCamera, roll);
		}
		cam.reset_pov(_base);
	} else if (cam.get_pos0().p != eye.p) {
		for (auto scenery = scenerys.begin(); scenery != End; ++scenery) {
			(*scenery)->recalculateLookat(_currentCamera, roll);
		}
		cam.reset_pov(eye);
	} else {
		for (auto scenery = scenerys.begin(); scenery != End; ++scenery) {
			(*scenery)->recalculateLookat(_currentCamera, roll);
		}
		cam.reset_pov(eye);
	}
}

void Scene::raytrasingCurrentCamera(void) {
	Camera&	cam(cameras[_currentCamera]);
	auto End = cam.matrix.end();
	for (auto pixel = cam.matrix.begin(); pixel != End; ++pixel) {
		trasingRay(pixel->ray, _currentCamera);
	}
}

A_Scenery* Scene::nearestIntersection(Ray& ray, int cam) {
	A_Scenery*	nearestObj = NULL;
	float		distance = INFINITY;
	auto		End = objsIdx.end();
	for (auto obj = objsIdx.begin(); obj != End; ++obj) {
		if ( (*obj)->intersection(ray, cam) ) {
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

bool Scene::shadow(Ray& ray, int cam) {
	Vec3f epsilon(ray.norm);
	epsilon.product(EPSILON);
	ray.pov.addition(ray.pov, epsilon);
	ray.dist += epsilon.norm();
	float	distance = ray.dist;
	auto	End = objsIdx.end();
	for (auto obj = objsIdx.begin(); obj != End; ++obj) {
		if ( (*obj)->intersection(ray, cam) ) {
			if (ray.dist < distance) {
				return true;
			}
		}
	}
	return false;
}

void Scene::trasingRay(Ray& ray, int cam) {
	if (ray.hits >= RECURSIONS) {
		return;
	}
	A_Scenery* obj = nearestIntersection(ray, cam);
	if (obj) {
		ray.hits++;
		ray.pov.addition( ray.pov, ray.dir * ray.dist ); // change ray.pov
		obj->getNormal(ray, cam);
		ray.color.product(obj->color, _ambient.light);// Ambient Lighting
		auto End = lightsIdx.end();
		for (auto light = lightsIdx.begin(); light != End; ++light) {
			if ( (*light)->lighting(ray, cam) && !shadow(ray, cam) ) {
				ray.light.product(obj->color, ray.light);
				ray.color.addition(ray.color, ray.light);
			}
		}
	} else {
		ray.color = _space.light;
	}
}

void Scene::rt(void) {
	raytrasingCurrentCamera();
	cameras[_currentCamera].takePicture(img);
	mlx_put_image_to_window(img.get_mlx(), img.get_win(), img.get_image(), 0, 0);
}

void Scene::selectCurrentCamera(int ctrl) {
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

void Scene::changeCurrentCameraFOV(int ctrl) {
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

void Scene::moveCurrentCamera(int ctrl) {
	Camera& cam(cameras[_currentCamera]);
	Position pos0(cam.get_pos0());
	switch (ctrl) {
		case MOVE_RIGHT:
			pos0.p.x += STEP_MOVE;
			break;
		case MOVE_LEFT:
			pos0.p.x -= STEP_MOVE;
			break;
		case MOVE_UP:
			pos0.p.y += STEP_MOVE;
			break;
		case MOVE_DOWN:
			pos0.p.y -= STEP_MOVE;
			break;
		case MOVE_FORWARD:
			pos0.p.z += STEP_MOVE;
			break;
		case MOVE_BACKWARD:
			pos0.p.z -= STEP_MOVE;
			break;
		default:
			break;
	}
	cam.reset_pov(pos0);
	rt();
}

void Scene::rotateCurrentCamera(int ctrl) {
	Camera&		cam(cameras[_currentCamera]);
	Position	eye(cam.get_pos0());
	switch (ctrl) {
		case YAW_RIGHT: {
			eye.n.z = std::cos(radian(STEP_ROTATION));
			eye.n.x = std::sin(radian(STEP_ROTATION));
			break;
		}
		case YAW_LEFT: {
			eye.n.z = std::cos(radian(-STEP_ROTATION));
			eye.n.x = std::sin(radian(-STEP_ROTATION));
			break;
		}
		case PITCH_UP: {
			eye.n.z = std::cos(radian(STEP_ROTATION));
			eye.n.y = std::sin(radian(STEP_ROTATION));
			break;
		}
		case PITCH_DOWN: {
			eye.n.z = std::cos(radian(-STEP_ROTATION));
			eye.n.y = std::sin(radian(-STEP_ROTATION));
			break;
		}
		case ROLL_RIGHT: {
			cam.reset_roll( cam.get_rollDegree() + STEP_ROTATION );
			break;
		}
		case ROLL_LEFT: {
			cam.reset_roll( cam.get_rollDegree() - STEP_ROTATION );
			break;
		}
		default:
			return;
	}
	recalculateLookatsForCurrentCamera(eye);
	rt();
}

void Scene::setFlybyRadiusForCurrentCamera(void) {
	Camera&		cam(cameras[_currentCamera]);
	Vec3f		pov = cam.get_pos0().p;
	float		tan = cam.get_fovTan();
	float		back = 0;
	float		front = INFINITY;

	auto End = cam.matrix.end();
	for (auto pixel = cam.matrix.begin(); pixel != End; ++pixel) {
		auto end = objsIdx.end();
		for (auto obj = objsIdx.begin(); obj != end; ++obj) {
			if ( (*obj)->intersection(pixel->ray, _currentCamera, FRONT) ) {
				if ( front > pixel->ray.dist ) {
					front = pixel->ray.dist;
				}
			}
			pixel->reset(tan, pov);
			if ( (*obj)->intersection(pixel->ray, _currentCamera, BACK) ) {
				if ( back < pixel->ray.dist && pixel->ray.dist < FLYBY_RADIUS_MAX) {
					back = pixel->ray.dist;
				}
			}
		}
	}
	if ( back > 0) {
		float r = (back - front) / 2 + front;
		cam.set_flybyRadius(r);
		if (DEBUG_MODE) {
			std::cout << "front: " << front << ", back: " << back;
			std::cout << ", flybyRadius: " << r << std::endl;
		}
	}
}

void Scene::flybyCurrentCamera(void) {
	Camera& cam(cameras[_currentCamera]);
	float angle = radian(FLYBY_STEP), radius = cam.get_flybyRadius();
	if (img.flyby == FLYBY_CLOCKWISE) {
		angle = -angle;
	}
	Position pos0(cam.get_pos0());
	pos0.p.z += radius;
	pos0.p.turnAroundY(angle);
	pos0.n.turnAroundY(-angle).normalize();
	pos0.p.z -= radius;
	recalculateLookatsForCurrentCamera(pos0);
	rt();
}


// Non member functions

std::ostream& operator<<(std::ostream& o, Scene& sc) {
	std::ostringstream os;
	o  << "R " << std::setw(5) << sc._resolution << " " << sc._header << std::endl;
	os << "A  " << std::setw(32) << sc._ambient;
	o << std::setw(36) << std::left << os.str();
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
