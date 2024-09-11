//
//  Camera.cpp
//  rt
//
//  Created by uru on 13/07/2024.
//

#include "camera.hpp"


// class FOV

Fov::Fov(void) : _degree(90.), _tan() { set_degree(_degree); }

Fov::~Fov(void) {}

Fov::Fov(const Fov& other) : _degree(other._degree), _tan(other._tan) {}

Fov& Fov::operator=(const Fov& other) {
	if (this != &other) {
		_degree = other._degree;
		_tan = other._tan;
	}
	return *this;
}

float Fov::get_degree(void) const { return _degree; }

float Fov::get_tan(void) const { return _tan; }

bool Fov::set_degree(float degree) {
	if (degree >= 0 && degree <= 180) {
		_degree = degree;
		_tan = std::tan( radian(_degree / 2) );
		return true;
	}
	return false;
}

bool Fov::set_tan(float tan) {
	bool b = set_degree( degree(std::atan(tan)) * 2 );
	return b && almostEqual(_tan, tan);
}


// Non member functions

std::ostream& operator<<(std::ostream& o, Fov& fov) {
	o << fov.get_degree();
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Fov& fov) {
	float f = 0;
	is >> f;
	if (!fov.set_degree(f)) {
		std::cerr	<< "The horizontal field of view in degrees should be in the range [0.180]. Stored value '"
					<< fov.get_degree() << "' instead of '" << f << "'" << std::endl;
	}
	return is;
}


// struct Pixel

Pixel::Pixel(const Vec3f& cPos, int sm, float tan, const Vec3f& pov) :
rays(), cPos(cPos), color() {
	reset(sm, tan, pov);
}

Pixel::~Pixel(void) {}

Pixel::Pixel(const Pixel& other) :
rays(other.rays),
cPos(other.cPos),
color(other.color)
{}

Pixel& Pixel::operator=(const Pixel& other) {
	if (this != &other) {
		rays = other.rays;
		cPos = other.cPos;
		color = other.color;
	}
	return *this;
}

void Pixel::reset(int sm, float tan, const Vec3f& pov) {
	rays.clear();
	rays.reserve(sm * sm);
	int sqrSm = sm * sm;
	for (int i = 0; i < sqrSm; i++) {
		rays.push_back(Ray());
	}
	restoreRays(sm, tan, pov);
}

void Pixel::restoreRays(int sm, float tan, const Vec3f& pov) {
	auto ray = rays.begin(), end = rays.end();
	for (int j = 0; j < sm; j++) {
		for (int i = 0; i < sm && ray != end; i++, ++ray) {
			ray->dir.x = (cPos.x + i * cPos.z) * tan;
			ray->dir.y = (cPos.y + j * cPos.z) * tan;
			ray->dir.z = 1;
			ray->dir.normalize();
			ray->pov = pov;
			ray->recursion = 0;
		}
	}
}

void Pixel::averageColor(void) {
	int k = (int)rays.size();
	if (k) {
		int a = 0, r = 0, g = 0, b = 0;
		for (auto ray = rays.begin(), end = rays.end(); ray != end; ++ray) {
			if (ray->shine.val != 0) {
				ray->color.addition(ray->color, ray->shine);
			}
			a += ray->color.a;
			r += ray->color.r;
			g += ray->color.g;
			b += ray->color.b;
			ray->shine = 0;
			ray->color = 0;
		}
		color.b = b / k;
		color.g = g / k;
		color.r = r / k;
		color.a = a / k;
	}
}


// class Matirx

Matrix::Matrix(void) :
_width(0),
_height(0),
_bytespp(0),
_mult(0),
_fov(),
_sm(SMOOTHING_FACTOR),
matrix()
{}

Matrix::~Matrix(void) {}

Matrix::Matrix(const Matrix& other) :
_width(other._width),
_height(other._height),
_bytespp(other._bytespp),
_mult(other._mult),
_fov(other._fov),
_sm(other._sm),
matrix(other.matrix)
{}

Matrix& Matrix::operator=(const Matrix& other) {
	if (this != &other) {
		_width = other._width;
		_height = other._height;
		_bytespp = other._bytespp;
		_mult = other._mult;
		_fov = other._fov;
		_sm = other._sm;
		matrix = other.matrix;
	}
	return *this;
}

int	Matrix::get_width(void) { return _width; }

int	Matrix::get_height(void) { return _height; }

int	Matrix::get_bytespp(void) { return _bytespp; }

Fov	Matrix::get_fov(void) { return _fov; }

float Matrix::get_fovDegree(void) { return _fov.get_degree(); }

float Matrix::get_fovTan(void) { return _fov.get_tan(); }

int   Matrix::get_sm(void) { return _sm; }

bool Matrix::set_fovDegree(float degree) { return _fov.set_degree(degree); }


// class Camera

Camera::Camera(const MlxImage& img) :
_base(BASE),
_pos(_base),
_roll(0),
_flybyRadius(0),
scenerys(),
objsIdx(),
lightsIdx(),
ambient(),
space(),
recursionDepth(RECURSION_DEPTH)
{	_width = img.get_width();
	_height = img.get_height();
	_bytespp = img.get_bytespp();
	_mult = 2. / _width;
}

Camera::~Camera(void) {}

Camera::Camera(const Camera& other) { *this = other; }

Camera& Camera::operator=(const Camera& other) {
	if (this != &other) {
		_base = other._base;
		_width = other._width;
		_height = other._height;
		_bytespp = other._bytespp;
		_mult = other._mult;
		_fov = other._fov;
		_sm = other._sm;
		matrix = other.matrix;
		_pos = other._pos;
		_roll = other._roll;
		_flybyRadius = other._flybyRadius;
		scenerys = other.scenerys;
		objsIdx = other.objsIdx;
		lightsIdx = other.lightsIdx;
		ambient = other.ambient;
		space = other.space;
		recursionDepth = other.recursionDepth;
	}
	return *this;
}

Position Camera::get_pos(void) const { return _pos; }

float Camera::get_rollDegree(void) const { return degree(_roll); }

float Camera::get_roll(void) const { return _roll; }

float Camera::get_flybyRadius(void) const { return _flybyRadius; }

int	 Camera::get_sm(void) const { return _sm; }

void Camera::set_scenery(A_Scenery* scenery) {
	scenerys.push_back(scenery);
	if ( scenerys.back()->get_isLight() == true ) {
		lightsIdx.push_back(scenery);
	} else {
		objsIdx.push_back(scenery);
	}
}

void Camera::set_pos(const Position& pos) { _pos = pos; }

void Camera::set_posToBase(void) { _pos = _base; }

void Camera::set_flybyRadius(float flybyRadius) { _flybyRadius = flybyRadius; }

void Camera::initMatrix(void) {
	Vec2i	mPos; // pixel xy-coordinate on the monitor (width*height pixels, xy(0,0) in the upper left corner, Y-axis direction down);
	Vec3f	cPos; // pixel xy-coordinate on the canvas (width=1, xy(0,0) in the center, XY-axes up and right directions)
	float	sm_mult = 1. / _sm;
	float	tan = _fov.get_tan();
	matrix.reserve(_width * _height);
	for (mPos.y = 0; mPos.y < _height; mPos.y++) {
		for (mPos.x = 0; mPos.x < _width; mPos.x++) {
			cPos.x = mPos.x;
			cPos.y = mPos.y;
			cPos.z = sm_mult;
			cPos = cPos.toRt(_width, _height) * _mult;
			matrix.push_back( Pixel(cPos, _sm, tan, _pos.p) );
		}
	}
}

void Camera::restoreRays(Camera* camera, unsigned long begin, unsigned long end) {
	camera->restoreRays_lll(begin, end);
}

void Camera::resetRays(Camera* camera, unsigned long begin, unsigned long end) {
	camera->resetRays_lll(begin, end);
}

void Camera::restoreRays_lll(unsigned long begin, unsigned long end) {
	float tan = _fov.get_tan();
	for (auto pixel = matrix.begin() + begin, _end = matrix.begin() + end; pixel != _end; ++pixel) {
		pixel->restoreRays(_sm, tan, _pos.p);
	}
}

void Camera::resetRays_lll(unsigned long begin, unsigned long end) {
	float sm_mult = (1. / _sm) * _mult;
	float tan = _fov.get_tan();
	for (auto pixel = matrix.begin() + begin, _end = matrix.begin() + end; pixel != _end; ++pixel) {
		pixel->cPos.z = sm_mult;
		pixel->reset(_sm, tan, _pos.p);
	}
}

bool Camera::resetFovDegree(float degree) {
	if (_fov.set_degree(degree)) {
		unsigned long size = this->matrix.size();
		unsigned long begin, end;
		std::thread th[NUM_THREADS];
		for (int i = 0; i < NUM_THREADS; i++) {
			begin = i * size / NUM_THREADS;
			if (i == NUM_THREADS - 1) {
				end = size;
			} else {
				end = size / NUM_THREADS * (i + 1);
			}
			th[i] = std::thread([this, begin, end](){restoreRays(this, begin, end);});
		}
		for (int i = 0; i < NUM_THREADS; i++) {
			th[i].join();
		}
		return true;
	}
	return false;
}

void Camera::resetSmoothingFactor(int sm) {
	_sm = sm;
	unsigned long size = this->matrix.size();
	unsigned long begin, end;
	std::thread th[NUM_THREADS];
	for (int i = 0; i < NUM_THREADS; i++) {
		begin = i * size / NUM_THREADS;
		if (i == NUM_THREADS - 1) {
			end = size;
		} else {
			end = size / NUM_THREADS * (i + 1);
		}
		th[i] = std::thread([this, begin, end](){resetRays(this, begin, end);});
	}
	for (int i = 0; i < NUM_THREADS; i++) {
		th[i].join();
	}
}

void Camera::resetRoll(float roll) {
	if (roll >= 90) {
		roll = radian(90);
	} else if (roll <= -90) {
		roll = radian(-90);
	} else if ( almostEqual(roll, 0, EPSILON) ) {
		roll = radian(0);
	} else {
		roll = radian(roll);
	}
	float shiftRoll = roll - _roll;
	_roll = roll;
	for (auto sc = scenerys.begin(), end = scenerys.end(); sc != end; ++sc) {
		(*sc)->roll(_pos.p, shiftRoll);
	}
	unsigned long size = this->matrix.size();
	unsigned long begin, end;
	std::thread th[NUM_THREADS];
	for (int i = 0; i < NUM_THREADS; i++) {
		begin = i * size / NUM_THREADS;
		if (i == NUM_THREADS - 1) {
			end = size;
		} else {
			end = size / NUM_THREADS * (i + 1);
		}
		th[i] = std::thread([this, begin, end](){restoreRays(this, begin, end);});
	}
	for (int i = 0; i < NUM_THREADS; i++) {
		th[i].join();
	}
	if (DEBUG) { std::cout << "roll: " << degree(_roll) << std::endl; }
}

void Camera::lookatCamera(const Position& pos) {
	LookatAux aux(pos.n);
	for (auto sc = scenerys.begin(), end = scenerys.end(); sc != end; ++sc) {
		(*sc)->lookat(pos, aux, _base.p, _roll);
	}
	set_posToBase();
	unsigned long size = this->matrix.size();
	unsigned long begin, end;
	std::thread th[NUM_THREADS];
	for (int i = 0; i < NUM_THREADS; i++) {
		begin = i * size / NUM_THREADS;
		if (i == NUM_THREADS - 1) {
			end = size;
		} else {
			end = size / NUM_THREADS * (i + 1);
		}
		th[i] = std::thread([this, begin, end](){restoreRays(this, begin, end);});
	}
	for (int i = 0; i < NUM_THREADS; i++) {
		th[i].join();
	}
}

void Camera::takePicture_lll(MlxImage& img, unsigned long begin, unsigned long end) {
	char* data = img.get_data();
	if (data) {
		data += _bytespp * begin;
		for (auto pixel = matrix.begin() + begin, _end = matrix.begin() + end; pixel != _end; ++pixel) {
			memcpy(data, pixel->color.raw, _bytespp);
			data += _bytespp;
		}
	}
}

void Camera::takePicture(Camera* camera, MlxImage& img, unsigned long begin, unsigned long end) {
	camera->takePicture_lll(img, begin, end);
}

void Camera::rayTracing_lll(unsigned long begin, unsigned long end) {
	for (auto pixel = matrix.begin() + begin, End = matrix.begin() + end; pixel != End; ++pixel) {
		for (auto ray = pixel->rays.begin(), _end = pixel->rays.end(); ray != _end; ++ray) {
			traceRay(*ray);
		}
		pixel->averageColor();
	}
}

void Camera::rayTracing(Camera* camera, unsigned long begin, unsigned long end) {
	camera->rayTracing_lll(begin, end);
}

void  Camera::traceRay(Ray& ray, int r) {
	if (r > recursionDepth) { return; }
	ray.recursion = r;
	A_Scenery* scenery = closestScenery(ray, _INFINITY);
	if (!scenery) { ray.color = space ; return; }
	ray.fixDirFromCam_if();
	scenery->giveNormal(ray);
	ray.movePovByNormal(EPSILON);
	lightings(ray, *scenery, r);	// order matters
	reflections(ray, *scenery, r);	// order matters
	refractions(ray, *scenery, r);	// order matters
}

void Camera::reflections(Ray& ray, const A_Scenery& scenery, int r) {
	if (scenery.reflective) {
		if (scenery.refractive && ray.hit == INSIDE) {
			return;
		}
		int _color = ray.color.val, _shine = ray.shine.val;
		ray.color = ray.shine = 0;
		if (scenery.refractive) {
			RaySafe raySafe(ray);
			ray.dir.reflect(ray.norm);
			traceRay(ray, ++r);
			ray.restore(raySafe);
		} else {
			ray.dir.reflect(ray.norm);
			traceRay(ray, ++r);
		}
		ray.collectReflectiveLight(_color, _shine, scenery.reflective);
	}
}
		
void Camera::refractions(Ray& ray, const A_Scenery& scenery, int r) {
	if (scenery.refractive) {
		int _color = ray.color.val;
		if (ray.dir.refract(ray.norm, ray.hit == INSIDE ? scenery.matIOR : scenery.matOIR)) {
			ray.color = 0;
			ray.movePovByNormal(-2 * EPSILON);
			traceRay(ray, ++r);
			ray.collectRefractiveLight(scenery.color, _color, scenery.refractive);
		} else {
			ray.collectLight(scenery.color);
		}
	}
}

void Camera::lightings(Ray& ray, const A_Scenery& scenery, int r) {
	ray.light = ambient;
	ray.collectLight(scenery.color);
	for (auto light = lightsIdx.begin(), end = lightsIdx.end(); light != end; ++light) {
		float k = (*light)->lighting(ray);
		if (k) {
			A_Scenery* shadow = closestScenery(ray, ray.dist, FRONT_SHADOW);
			if (shadow) {
				transparentShadow(ray, *shadow, scenery, k, r);
			}  else {
				ray.collectLight(scenery.color, k);
				ray.collectShine(scenery.specular);
			}
		}
	}
}

bool Camera::transparentShadow(Ray& ray, const A_Scenery& shadow, const A_Scenery& scenery, float k, int r) {
	if (shadow.refractive) {
		RaySafe	raySafe(ray);
		ray.dir = ray.dirToLight;
		shadow.giveNormal(ray);
		if (ray.dir.refract(ray.norm, ray.hit == INSIDE ? shadow.matIOR : shadow.matOIR)) {
			ColorsSafe	colorsSafe(ray);
			ray.movePovByNormal(EPSILON);
			traceRay(ray, ++r);
			ray.collectRefractiveLight(shadow.color, colorsSafe.color, shadow.refractive);
			ray.collectShadowLight(colorsSafe, scenery.color, k);
			ray.restore(raySafe);
			return true;
		}
		ray.restore(raySafe);
	}
	return false;
}

A_Scenery* Camera::closestScenery(Ray& ray, float distance, Hit hit) {
	A_Scenery*	closestScenery = NULL;
	Hit			rayHit = hit;
	auto 		scenery = scenerys.begin(), end = scenerys.end();
	while (scenery != end) {
		if ( (*scenery)->combineType == END) {
			if ( (*scenery)->intersection(ray.set_hit(hit)) && distance > ray.dist ) {
				distance = ray.dist;
				rayHit = ray.hit;
				closestScenery = *scenery;
			}
		} else {
			ray.combineStart(*scenery, hit);
			++scenery;
			while (scenery != end) {
				ray.combineNext(*scenery, hit);
				if ( (*scenery)->combineType == END ) {
					break;
				}
				++scenery;
			}
			A_Scenery* combineGet = ray.combineGet();
			if ( combineGet && distance > ray.dist ) {
				distance = ray.dist;
				rayHit = ray.hit;
				closestScenery = combineGet;
			}
		}
		if (scenery != end) ++scenery;
	}
	if (closestScenery) {
		ray.dist = distance;
		ray.hit = rayHit;
	}
	return closestScenery;
}

void Camera::calculateFlybyRadius(void) {
	float	back = 0;
	float	front = FLYBY_RADIUS_MAX;
	for (auto pixel = matrix.begin(), END = matrix.end(); pixel != END; ++pixel) {
		pixel->restoreRays(_sm, _fov.get_tan(), _pos.p);
		for (auto ray = pixel->rays.begin(), End = pixel->rays.end(); ray != End; ++ray) {
			for (auto sc = objsIdx.begin(), end = objsIdx.end(); sc != end; ++sc) {
				ray->hit = FRONT;
				if ( (*sc)->intersection(*ray) ) {
					if (front > ray->dist) {
						front = ray->dist;
					}
				}
				ray->hit = BACK;
				if ( (*sc)->intersection(*ray) ) {
					if (back < ray->dist && ray->dist < FLYBY_RADIUS_MAX) {
						back = ray->dist;
					}
				}
			}
		}
	}
	if ( back > 0) {
		_flybyRadius = (back - front) / 2 + front;
		if (DEBUG_MODE) {
			std::cout << "front: " << front << ", back: " << back;
			std::cout << ", flybyRadius: " << _flybyRadius << std::endl;
		}
	}
}


// Non member functions

std::ostream& operator<<(std::ostream& o, Camera& camera) {
	std::ostringstream os;
	os << std::setw(2) << std::left << camera._nick;
	os << " " << camera._pos.p;
	os << " " << camera._pos.n;
	os << " " << std::setw(4) << camera._fov.get_degree();
	o << std::setw(56) << std::left << os.str();
	o << " #" << camera._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Camera& camera) {
	is >> camera._pos.p >> camera._pos.n >> camera._fov;
	camera._pos.n.normalize();
	return is;
}

