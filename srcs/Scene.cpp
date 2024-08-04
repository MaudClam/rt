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

	set_any( std::istringstream("c     0,0,-2         0,0,1      60 ") );
	set_any( std::istringstream("c     5,0,3         -1,0,0      60 ") );
	set_any( std::istringstream("c     0,0,8         0,0,-1      60 ") );
	set_any( std::istringstream("c     -5,0,3        1,0,0      60 ") );
	set_any( std::istringstream("c     0,4,2         0,-1,0      60 ") );
	set_any( std::istringstream("A 0.2	255,255,250") );
	set_any( std::istringstream("l     2,1,0    0.6 " + img.white.rrggbb() + "  0,0,0 ") );
	set_any( std::istringstream("l     inf,0,0  0.2 " + img.white.rrggbb() + "  1,4,4 ") );
	set_any( std::istringstream("sp    0,-1,3	2   " + img.red.rrggbb()   + " 500  0.2") );
	set_any( std::istringstream("sp    2,0,4	2   " + img.blue.rrggbb()  + " 500  0.3") );
	set_any( std::istringstream("sp    -2,0,4	2   " + img.green.rrggbb() + " 10  0.4") );
	set_any( std::istringstream("sp 0,-5001,0 10000 " + img.yellow.rrggbb()+ " 1000 0.5") );

//	===========
	
	if (cameras.size() > 1) {
		_currentCamera = 1;
	}
	if (DEBUG) { std::cout << *this; }
	return SUCCESS;
}

int  Scene::get_currentCamera(void) { return _currentCamera;}

bool Scene::set_currentCamera(int idx) {
	if (idx >= 0 && idx < (int)cameras.size()) {
		_currentCamera = idx;
		cameras[_currentCamera].resetRays();
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
		case 3: {// c litght
			Light* l = new Light();
			is >> *l;
			set_scenery(l);
			break;
		}
		case 4: {// sp sphere
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

void Scene::raytrasing(void) {
	Camera&	cam(cameras[_currentCamera]);
	for (auto pixel = cam.matrix.begin(), End = cam.matrix.end(); pixel != End; ++pixel) {
		for (auto ray = pixel->rays.begin(), end = pixel->rays.end(); ray != end; ++ray) {
			trasingRay(*ray, 0);
		}
		pixel->averageColor();
	}
}

A_Scenery* Scene::nearestIntersection(Ray& ray) {
	A_Scenery*	nearestObj = NULL;
	float		distance = INFINITY;
	auto		End = objsIdx.end();
	for (auto obj = objsIdx.begin(); obj != End; ++obj) {
		if ( (*obj)->intersection(ray) ) {
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
	Vec3f epsilon(ray.norm);
	epsilon.product(EPSILON);
	ray.pov.addition(ray.pov, epsilon);
	float	distance = ray.dist;
	auto	End = objsIdx.end();
	for (auto obj = objsIdx.begin(); obj != End; ++obj) {
		if ( (*obj)->intersection(ray) ) {
			if (ray.dist < distance) {
				return true;
			}
		}
	}
	return false;
}

void Scene::trasingRay(Ray& ray, int recursion) {
	if (recursion > RECURSIONS) {
		return;
	}
	A_Scenery* obj = nearestIntersection(ray);
	if (obj) {
		ray.color.product(obj->color, _ambient.light);// Ambient Lighting
		ray.pov.addition( ray.pov, ray.dir * ray.dist ); // change ray.pov
		obj->getNormal(ray);
		Ray rRay(ray); rRay.reflect();
		Vec3f V(ray.dir); V.product(-1);
		auto End = lightsIdx.end();
		for (auto light = lightsIdx.begin(); light != End; ++light) {
			if ( (*light)->lighting(ray) && !shadow(ray) ) {
				ray.light.product(obj->color, ray.light);
				ray.color.addition(ray.color, ray.light);
				if (obj->specular != -1) {
					Vec3f r(reflectRay(ray.norm, ray.dir));
					float k = r * V;
					if (k > 0) {
						k = std::pow(k, obj->specular);
						ray.light.product(obj->color, ray.light).product(k);
						ray.color.addition(ray.color, ray.light);
					}
				}
			}
		}
		if (obj->reflective > 0) {
			trasingRay(rRay, ++recursion);
			ray.color.addition(ray.color.product(1 - obj->reflective), rRay.color.product(obj->reflective));
		}
	} else {
		ray.color = _space.light;
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
	LookatAux aux(pos.n);
	Vec3f	  shift;
	switch (ctrl) {
		case MOVE_RIGHT:
			shift.x = STEP_MOVE;
			break;
		case MOVE_LEFT:
			shift.x = -STEP_MOVE;
			break;
		case MOVE_UP:
			shift.y = STEP_MOVE;
			break;
		case MOVE_DOWN:
			shift.y = -STEP_MOVE;
			break;
		case MOVE_FORWARD:
			shift.z = STEP_MOVE;
			break;
		case MOVE_BACKWARD:
			shift.z = -STEP_MOVE;
			break;
		default:
			break;
	}
	shift.lookatPt(Vec3f(), aux);
	pos.p = pos.p + shift;
	cam.reset_pos(pos);
	rt();
}

void Scene::rotateCamera(int ctrl) {
	Camera&   cam(cameras[_currentCamera]);
	Position  pos(cam.get_pos());
	LookatAux aux(pos.n);
	Vec3f	  shift(0,0,1);
	switch (ctrl) {
		case YAW_RIGHT:
			shift.turnAroundY(radian(STEP_ROTATION));
			break;
		case YAW_LEFT:
			shift.turnAroundY(radian(-STEP_ROTATION));
			break;
		case PITCH_UP:
			shift.turnAroundX(radian(-STEP_ROTATION));
			break;
		case PITCH_DOWN:
			shift.turnAroundX(radian(STEP_ROTATION));
			break;
		case ROLL_RIGHT:
			shift.turnAroundZ(radian(STEP_ROTATION));
			break;
		case ROLL_LEFT:
			shift.turnAroundZ(radian(-STEP_ROTATION));
			break;
		default:
			break;
	}
	shift.lookatDir(aux);
	pos.n.addition(pos.n,shift).normalize();
	cam.reset_pos(pos);
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
