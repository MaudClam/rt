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

void Scene::set_currentCamera(int cameraIdx) {
	if (checkCameraIdx(cameraIdx)) {
		this->currentCamera = cameraIdx;
	}
}

int	Scene::parsing(int ac, char** av) {
	(void)ac; (void)av;
	Vec2i	resolution(800,600);

	img.init("Hello", resolution.x, resolution.y);
	set_camera(Camera(img));// default camera should stay always
	set_camera( std::istringstream("c 0,0,0 0,0,1 70") );
//	set_camera(img);
//	set_camera(img);
//	set_camera(img);

	Sphere*	sp1 = new Sphere(Vec3f(-5.1,2,45), 6, img.lightGray);
	Sphere*	sp2 = new Sphere(Vec3f(0,-1,3), 1, img.red);
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
			(*scenery)->set_lookat(camera->get_pos());
		}
	}
	if (cameras.size() > 1) {
		currentCamera = 1;
	}
}

void Scene::resetCamera(int cameraIdx) {
	if (checkCameraIdx(cameraIdx)) {
		cameras[cameraIdx].resetPixels();
	}
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
		(*scenery)->recalculateLookat(currentCamera, cameras[currentCamera].get_pos());
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

void Scene::nextCamera(void) {

	if (cameras.size() <= 2) {
		if (DEBUG_MODE) { std::cout << "currentCamera: " << currentCamera << "\n";}
		return;
	} else if ( currentCamera >= (int)(cameras.size() - 1) ) {
		currentCamera = 1;
	} else {
		currentCamera++;
	}
	resetCamera(currentCamera);
	if (DEBUG_MODE) { std::cout << "currentCamera: " << currentCamera << "\n";}
}

void Scene::previousCamera(void) {
	if (cameras.size() <= 2) {
		if (DEBUG_MODE) { std::cout << "currentCamera: " << currentCamera << "\n";}
		return;
	} else if ( currentCamera <= 1 ) {
		currentCamera = (int)(cameras.size() - 1);
	} else {
		currentCamera--;
	}
	if (DEBUG_MODE) { std::cout << "currentCamera: " << currentCamera << "\n";}
}

void Scene::chooseCamera(int i) {
	if ( i >= 0 && i < (int)cameras.size() ) {
		currentCamera = i;
	}
	if (DEBUG_MODE) { std::cout << "===currentCamera: " << currentCamera << "\n";}
}
