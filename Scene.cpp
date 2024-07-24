//
//  Scene.cpp
//  rt
//
//  Created by uru on 12/07/2024.
//

#include "Scene.hpp"


// Class Scene

Scene::Scene(MlxImage& img) :
base(BASE),
img(img),
scenerys(),
objsIdx(),
lightsIdx(),
cameras(),
_resolution(800,600),
_header(std::to_string(_resolution.x) + "x" + std::to_string(_resolution.y)),
_ambient(1),
_space(1),
_currentCamera(0) {
	img.set_scene(this);
	_space.invert();
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
base(other.base),
img(other.img),
scenerys(other.scenerys),
objsIdx(other.objsIdx),
lightsIdx(other.lightsIdx),
cameras(other.cameras),
_currentCamera(other._currentCamera)
{}

Scene& Scene::operator=(const Scene& other) {
	if (this != &other) {
		scenerys = other.scenerys;
		objsIdx = other.objsIdx;
		lightsIdx = other.lightsIdx;
		cameras = other.cameras;
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

void Scene::set_scenery(AScenery* scenery) {
	scenerys.push_back(scenery);
	if ( scenerys.back()->get_light() == true ) {
		lightsIdx.push_back(scenery);
	} else {
		objsIdx.push_back(scenery);
	}
}

void Scene::set_ambientLight(std::istringstream is) {
	is >> _ambient;
	_space = _ambient;
	_space.invert();
}

void Scene::set_camera(const Camera& camera) {
	cameras.push_back(camera);
}

void Scene::set_camera(std::istringstream is) {
	cameras.push_back(Camera(img));
	is >> cameras.back();
}

int  Scene::parsing(int ac, char** av) {
	(void)ac; (void)av;
	Vec2i	resolution(800,600);
	set_ambientLight( std::istringstream("0.9 255,255,255") );

	img.init(_header, resolution.x, resolution.y);
	set_camera(Camera(img));// default camera should stay always
	BasicCoordinate* basicCoordinate = new BasicCoordinate();
	set_scenery(basicCoordinate);
	
	
	set_camera( std::istringstream("c 0,0,0 0,0,1 90") );
	set_camera( std::istringstream("c 0,0,38 0,0,-1 60") );
	set_camera( std::istringstream("c 19,0,19 -1,0,0 60") );
//	set_camera(Camera(img));
//	set_camera(Camera(img));

	Sphere*	sp1 = new Sphere(Vec3f(0,0,25), 8, img.lightGRay_);
	Sphere*	sp2 = new Sphere(Vec3f(0,0,16.9), 4, img.yellow);
	Sphere*	sp3 = new Sphere(Vec3f(0,0,12.5), 2, img.cyan);
	Sphere*	sp4 = new Sphere(Vec3f(0,0,9.9), 1, img.magenta);
//	Sphere*	sp5 = new Sphere(Vec3f(0,0,3), 0.5, img.red);
	set_scenery(sp1);
	set_scenery(sp2);
	set_scenery(sp3);
	set_scenery(sp4);
//	set_scenery(sp_5);

	
//	===========
	
	initLoockats();
	return SUCCESS;
}

void Scene::indexingScenerys(void) {
	objsIdx.clear();
	lightsIdx.clear();
	auto End = scenerys.end();
	for (auto scenery = scenerys.begin(); scenery != End; ++scenery) {
		if ( (*scenery)->get_light() ) {
			lightsIdx.push_back(*scenery);
		} else {
			objsIdx.push_back(*scenery);
		}
	}
}

void Scene::initLoockats(void) {
	if ( scenerys.size() < 1 || (*scenerys[0]).get_nick().compare(0, 2, "BC")) {
		std::cerr << "Warning: BasicCoordinate class object is missing." << std::endl;
	}
	auto End = cameras.end();
	for (auto camera = cameras.begin(); camera != End; ++camera) {
		Position eye(camera->get_pos());
		if (base.n != eye.n) {
			LookatAux aux(eye.n);
			auto end = scenerys.end();
			for (auto scenery = scenerys.begin(); scenery != end; ++scenery ) {
				(*scenery)->set_lookatCamera(eye, aux);
			}
			camera->reset_pov(base);
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
	if (cameraIdx >= 0 && cameraIdx < cameras.size()) {
		return true;
	}
	if (DEBUG_MODE) {
		std::cerr	<< "CameraIdx '" << cameraIdx << "' is out of range, "
		<< "saved current camera '" << _currentCamera << "'" << std::endl;
	}
	return false;
}

void Scene::recalculateLookatsForCurrentCamera(const Position& eye) {
	Camera&	camera(cameras[_currentCamera]);
	if (base.n != eye.n) {
		LookatAux aux(eye.n);
		auto End = scenerys.end();
		for (auto scenery = scenerys.begin(); scenery != End; ++scenery) {
			(*scenery)->recalculateLookat(_currentCamera, eye, aux);
		}
		camera.reset_pov(base);
	} else if (camera.get_pos0().p != eye.p) {
		camera.reset_pov(eye);
	}
}

void Scene::raytrasingCurrentCamera(void) {
	Camera&	cam(cameras[_currentCamera]);
	auto End = cam.matrix.end();
	for (auto pixel = cam.matrix.begin(); pixel != End; ++pixel) {
		trasingRay(pixel->ray, _currentCamera, cam.get_roll());
	}
}

void Scene::trasingRay(Ray& ray, int cam, float roll) {
	float		distance = INFINITY;
	AScenery*	nearestObj = NULL;
	auto End = objsIdx.end();
	for (auto obj = objsIdx.begin(); obj != End; ++obj) {
		if ( (*obj)->intersection(ray, cam, roll) ) {
			if (distance > ray.dist) {
				distance = ray.dist;
				nearestObj = *obj;
			}
		}
	}
	if (nearestObj) {
		ray.dist = distance;
		nearestObj->hit(ray, cam, roll);
		lighting(ray, cam, roll);
	} else {
		ray.color = _space.light;
	}
}

void Scene::lighting(Ray& ray, int cam, float roll) {
	ray.color.product(ray.color, _ambient.light);
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
	Camera& cam(cameras[_currentCamera]);
	Position eye(cam.get_pos0());
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
			cam.reset_roll( cam.get_rollDegree() - STEP_ROTATION );
			rt();
			return;
		}
		case ROLL_LEFT: {
			cam.reset_roll( cam.get_rollDegree() + STEP_ROTATION );
			rt();
			return;
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
			if ( (*obj)->intersection(pixel->ray, _currentCamera, 0, AScenery::FRONT) ) {
				if ( front > pixel->ray.dist ) {
					front = pixel->ray.dist;
				}
			}
			pixel->reset(tan, pov);
			if ( (*obj)->intersection(pixel->ray, _currentCamera, 0, AScenery::BACK) ) {
				if ( back < pixel->ray.dist ) {
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
	o	<< "R"
		<< " " << sc._resolution.x
		<< " " << sc._resolution.x
		<< " " << sc._header
		<< "\t#Resolution";
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Scene& sc) {
	if (!is.str().compare(0,1,"R")) {
		char trash;
		is >> trash >> sc._resolution.x >> sc._resolution.y;
		is >> sc._header;
	}
	return is;
}
