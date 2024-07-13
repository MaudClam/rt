//
//  Scene.cpp
//  rt
//
//  Created by uru on 12/07/2024.
//

#include "Scene.hpp"

extern Var var;

Scene::Scene(void) : scenerys(), objsIdx(), lightsIdx() {
	Sphere*	sp1 = new Sphere(Vec3f(-5.1,2,45), 6, var.img->lightGray);
	Sphere*	sp2 = new Sphere(Vec3f(0,-1,3), 1, var.img->red);
	Sphere*	sp3 = new Sphere(Vec3f(2,0,4), 1, var.img->blue);
	Sphere*	sp4 = new Sphere(Vec3f(-2,0,4), 1, var.img->green);

	set_scenery(sp1);
	set_scenery(sp2);
	set_scenery(sp3);
	set_scenery(sp4);
}

Scene::~Scene(void) {
	for (auto scenery = scenerys.begin(); scenery != scenerys.end(); ++scenery) {
		delete *scenery;
		*scenery = NULL;
	}
	if (DEBUG_MODE) { std::cout << "~Scene() destructor was done.\n"; }
}

Scene::Scene(const Scene& other) :
scenerys(other.scenerys),
objsIdx(other.objsIdx),
lightsIdx(other.lightsIdx)
{}

Scene& Scene::operator=(const Scene& other) {
	if (this != &other) {
		scenerys = other.scenerys;
		objsIdx = other.objsIdx;
		lightsIdx = other.lightsIdx;
	}
	return *this;
}

void Scene::set_scenery(AScenery* scenery) {
	scenerys.push_back(scenery);
	if ( scenerys.back()->get_light() ) {
		lightsIdx.push_back(scenery);
	} else {
		objsIdx.push_back(scenery);
	}
}

void Scene::indexing(void) {
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

