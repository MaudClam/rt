//
//  Scene.cpp
//  rt
//
//  Created by uru on 12/07/2024.
//

#include "Scene.hpp"

Scene::Scene(MlxImage& img) :
img(img),
scenerys(),
objsIdx(),
lightsIdx(),
cameras(),
currentCamera(0) { img.set_scene(this); }

Scene::~Scene(void) {
	for (auto scenery = scenerys.begin(); scenery != scenerys.end(); ++scenery) {
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
currentCamera(other.currentCamera)
{}

Scene& Scene::operator=(const Scene& other) {
	if (this != &other) {
		scenerys = other.scenerys;
		objsIdx = other.objsIdx;
		lightsIdx = other.lightsIdx;
		cameras = other.cameras;
		currentCamera = other.currentCamera;
	}
	return *this;
}

int  Scene::get_currentCamera(void) { return currentCamera;}

bool Scene::set_currentCamera(int cameraIdx) {
	if (checkCameraIdx(cameraIdx)) {
		this->currentCamera = cameraIdx;
		if (DEBUG_MODE) { std::cout << "currentCamera: " << currentCamera << "\n";}
		return true;
	}
	return false;
}

int	Scene::parsing(int ac, char** av) {
	(void)ac; (void)av;
	Vec2i	resolution(800,600);

	img.init("Hello", resolution.x, resolution.y);
	set_camera(Camera(img));// default camera should stay always
	set_camera( std::istringstream("c 5,0,4 -1,0,0 70") );
	set_camera( std::istringstream("c 0,0,0 0,0,1 70") );
	set_camera(Camera(img));
	set_camera(Camera(img));

	Sphere*	sp1 = new Sphere(Vec3f(0,0,45), 6, img.lightGray);
	Sphere*	sp2 = new Sphere(Vec3f(0,0,3), 1, img.red);
	Sphere*	sp3 = new Sphere(Vec3f(2,0,4), 1, img.blue);
	Sphere*	sp4 = new Sphere(Vec3f(-2,0,4), 1, img.green);
	set_scenery(sp1);
	set_scenery(sp2);
	set_scenery(sp3);
	set_scenery(sp4);
	
	initCameras();
	return SUCCESS;
}

void Scene::set_scenery(AScenery* scenery) {
	scenerys.push_back(scenery);
	if ( scenerys.back()->get_light() ) {
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

void Scene::initCameras(void) {
	for (auto camera = cameras.begin(); camera != cameras.end(); ++camera) {
		camera->initPixels();
		for (auto scenery = scenerys.begin(); scenery != scenerys.end(); ++scenery ) {
			(*scenery)->set_lookat(camera->get_pos(), camera->get_roll());
		}
	}
	if (cameras.size() > 1) {
		currentCamera = 1;
	}
}

void Scene::resetCurrentCamera(void) {
	cameras[currentCamera].resetPixels();
}

bool Scene::checkCameraIdx(int cameraIdx) const {
	if (cameraIdx >= 0 && cameraIdx < cameras.size()) {
		return true;
	}
	if (DEBUG_MODE) {
		std::cerr	<< "CameraIdx '" << cameraIdx << "' is out of range, "
		<< "saved current camera '" << currentCamera << "'" << std::endl;
	}
	return false;
}

void Scene::recalculateLookatsForCurrentCamera(void) {
	for (auto scenery = scenerys.begin(); scenery != scenerys.end(); ++scenery) {
		(*scenery)->recalculateLookat(currentCamera, cameras[currentCamera].get_pos(), cameras[currentCamera].get_roll());
	}
}

void Scene::raytraisingCurrentCamera(void) {
	for (auto pixel = cameras[currentCamera].pixels.begin(); pixel != cameras[currentCamera].pixels.end(); ++pixel) {
		for (auto obj = objsIdx.begin(); obj != objsIdx.end(); ++obj) {
			if ( (*obj)->intersection(*pixel, currentCamera) && pixel->dist < INFINITY) {
				pixel->color = (*obj)->color;
			}
		}
	}
}

void Scene::rt(void) {
	raytraisingCurrentCamera();
	putCurrentCameraPixelsToImg();
	mlx_put_image_to_window(img.get_mlx(), img.get_win(), img.get_image(), 0, 0);
}

void Scene::putCurrentCameraPixelsToImg(void) {
	for (auto pixel = cameras[currentCamera].pixels.begin(); pixel != cameras[currentCamera].pixels.end(); ++pixel) {
		pixel->drawPixel();
	}
}

void Scene::selectCurrentCamera(int ctrl) {
	switch (ctrl) {
		case NEXT: {
			if (set_currentCamera(currentCamera +  1))
				rt();
			break;
		}
		case PREVIOUS: {
			if (set_currentCamera(currentCamera -  1))
				rt();
			break;
		}
		default: {
			if (set_currentCamera(ctrl))
				rt();
			break;
		}
	}
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
	cameras[currentCamera].move(ctrl);
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
