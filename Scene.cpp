//
//  Scene.cpp
//  rt
//
//  Created by uru on 12/07/2024.
//

#include "Scene.hpp"

Scene::Scene(MlxImage& img) :
base(BASE),
img(img),
scenerys(),
objsIdx(),
lightsIdx(),
cameras(),
_currentCamera(0) { img.set_scene(this); }

Scene::~Scene(void) {
	for (auto scenery = scenerys.begin(); scenery != scenerys.end(); ++scenery) {
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
		if (DEBUG_MODE) { std::cout << "currentCamera: " << _currentCamera << "\n";}
		return true;
	}
	return false;
}

int  Scene::parsing(int ac, char** av) {
	(void)ac; (void)av;
	Vec2i	resolution(1200,900);

	img.init("Hello", resolution.x, resolution.y);
	set_camera(Camera(img));// default camera should stay always
	BasicCoordinate* basicCoordinate = new BasicCoordinate();
	set_scenery( basicCoordinate );
	
	
//	set_camera( std::istringstream("c 5,0,4 -1,0,0 70") );
	set_camera( std::istringstream("c 0,0,0 0,0,1 90") );
//	set_camera(Camera(img));
//	set_camera(Camera(img));

//	Sphere*	sp1 = new Sphere(Vec3f(0,0,45), 6, img.lightGRay_);
	Sphere*	sp2 = new Sphere(Vec3f(0,0,12), 4, img.red);
	Sphere*	sp3 = new Sphere(Vec3f(0,0,6), 2, img.blue);
	Sphere*	sp4 = new Sphere(Vec3f(0,0,3), 1, img.green);
//	set_scenery(sp1);
	set_scenery(sp2);
	set_scenery(sp3);
	set_scenery(sp4);
	
	
//	===========
	
	initLoockats();
	return SUCCESS;
}

void Scene::set_scenery(AScenery* scenery) {
	scenerys.push_back(scenery);
	if ( scenerys.back()->get_light() == true ) {
		lightsIdx.push_back(scenery);
	} else {
		objsIdx.push_back(scenery);
	}
}

void Scene::set_camera(const Camera& camera) {
	cameras.push_back(camera);
}

void Scene::set_camera(std::istringstream is) {
	cameras.push_back(Camera(img));
	is >> cameras.back();
}

void Scene::indexingScenerys(void) {
	objsIdx.clear();
	lightsIdx.clear();
	for (auto scenery = scenerys.begin(); scenery != scenerys.end(); ++scenery) {
		if ( (*scenery)->get_light() ) {
			lightsIdx.push_back(*scenery);
		} else {
			objsIdx.push_back(*scenery);
		}
	}
}

void Scene::initLoockats(void) {
	if ( scenerys.size() < 1 || (*scenerys[0]).get_nick().compare(0, 2, "BS")) {
		std::cerr << "Warning: BasicCoordinate class object is missing." << std::endl;
	}
	for (auto camera = cameras.begin(); camera != cameras.end(); ++camera) {
		Position eye(camera->get_pos());
		if ( base.n == eye.n ) {
			for (auto scenery = scenerys.begin(); scenery != scenerys.end(); ++scenery ) {
				(*scenery)->set_lookatBase();
			}
			camera->reset_pov(eye.p);
		} else {
			LookatAuxiliary aux(eye.n);
			for (auto scenery = scenerys.begin(); scenery != scenerys.end(); ++scenery ) {
				(*scenery)->set_lookatCamera(eye, aux);
			}
			camera->reset_pov(base.p);
		}
	}
	if (cameras.size() > 1) {
		_currentCamera = 1;
	}
}

//void Scene::resetCurrentCamera(void) {
//	cameras[_currentCamera].resetPixels();
//}

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

bool Scene::recalculateLookatsForCurrentCamera(const Position& eye) {
	Camera&	camera(cameras[_currentCamera]);
	if (camera.get_pos0().n != eye.n) {
		camera.set_pos0(base);
		LookatAuxiliary aux(eye.n);
		for (auto scenery = scenerys.begin(); scenery != scenerys.end(); ++scenery) {
			(*scenery)->recalculateLookat(_currentCamera, eye, aux);
		}
		camera.reset_pov(eye.p);
		return true;
	}
	return camera.reset_pov(eye.p);
}

void Scene::raytrasingCurrentCamera(void) {
	float	dist = INFINITY;
	Camera&	camera(cameras[_currentCamera]);
	for (auto pixel = camera.matrix.begin(); pixel != camera.matrix.end(); ++pixel) {
		for (auto obj = objsIdx.begin(); obj != objsIdx.end(); ++obj) {
			if ( (*obj)->intersection(pixel->ray, _currentCamera)) {
				if (pixel->ray.dist < dist) {
					pixel->ray.dist = dist;
					pixel->ray.color = (*obj)->color;
				}
			}
		}
	}
}

void Scene::rt(void) {
	raytrasingCurrentCamera();
	putCurrentCameraPixelsToImg();
	mlx_put_image_to_window(img.get_mlx(), img.get_win(), img.get_image(), 0, 0);
}

void Scene::putCurrentCameraPixelsToImg(void) {
	Camera&	camera(cameras[_currentCamera]);
	for (auto pixel = camera.matrix.begin(); pixel != camera.matrix.end(); ++pixel) {
		pixel->drawPixel();
	}
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
	Camera& cam(cameras[currentCamera]);
	switch (ctrl) {
		case INCREASE_FOV: {
			if ( cam.set_fov(cam.get_fov() + STEP_FOV) ) {
				resetCurrentCamera();
				rt();
			}
			break;
		}
		case DECREASE_FOV: {
			if ( cam.set_fov(cam.get_fov() - STEP_FOV) ) {
				resetCurrentCamera();
				rt();
			}
			break;
		}
		default: {
			if ( cam.set_fov(ctrl) ) {
				resetCurrentCamera();
				rt();
			}
			break;
		}
	}
}

void Scene::moveCurrentCamera(int ctrl) {
	if (img.flyby == FLYBY_ON) {
		cameras[currentCamera].flyby(ctrl);
	} else {
		cameras[currentCamera].move(ctrl);
	}
	recalculateLookatsForCurrentCamera();
	resetCurrentCamera();
	rt();
}

void Scene::rotateCurrentCamera(int ctrl) {
	cameras[currentCamera].rotate(ctrl);
	recalculateLookatsForCurrentCamera();
	resetCurrentCamera();
	rt();
}

void Scene::setFlybyRadiusForCurrentCamera(void) {
	Ray_		Ray_;
	float	back = 0;
	float	front = 0;
	for (auto obj = objsIdx.begin(); obj != objsIdx.end(); ++obj) {
		if ( (*obj)->intersection(Ray_, currentCamera, AScenery::BACK) ) {
			if ( Ray_.dist > back) {
				back = Ray_.dist;
			}
		}
	}
	for (auto obj = objsIdx.begin(); obj != objsIdx.end(); ++obj) {
		if ( (*obj)->intersection(Ray_, currentCamera, AScenery::FRONT) ) {
			if ( Ray_.dist < front) {
				front = Ray_.dist;
			}
		}
	}
	if ( back != 0) {
		cameras[currentCamera].set_flybyRadius((back + front) / 2);
		if (DEBUG_MODE) { std::cout << "flybyRadius: " << cameras[currentCamera].get_flybyRadius() << std::endl; }
	}
}
