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
	if (degree >= 0 && degree <= 180 && !almostEqual(degree, _degree, EPSILON)) {
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
		}
	}
}

void Pixel::averageColor(void) {
	int k = (int)rays.size();
	if (k) {
		int a = 0, r = 0, g = 0, b = 0;
		for (auto ray = rays.begin(), end = rays.end(); ray != end; ++ray) {
			a += ray->color.a;
			r += ray->color.r;
			g += ray->color.g;
			b += ray->color.b;
			ray->color.val = 0;
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
space()
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

void Camera::restoreRays_lll(void) {
	float tan = _fov.get_tan();
	for (auto pixel = matrix.begin(), end = matrix.end(); pixel != end; ++pixel) {
		pixel->restoreRays(_sm, tan, _pos.p);
	}
}

void Camera::resetRays_lll(void) {
	float sm_mult = (1. / _sm) * _mult;
	float tan = _fov.get_tan();
	for (auto pixel = matrix.begin(), end = matrix.end(); pixel != end; ++pixel) {
		pixel->cPos.z = sm_mult;
		pixel->reset(_sm, tan, _pos.p);
	}
}

bool Camera::resetFovDegree(float degree) {
	if (_fov.set_degree(degree)) {
		restoreRays_lll();
		return true;
	}
	return false;
}

void Camera::resetSmoothingFactor(int sm) {
	_sm = sm;
	resetRays_lll();
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
	restoreRays_lll();
	if (DEBUG) { std::cout << "roll: " << degree(_roll) << std::endl; }
}

void Camera::lookatCamera(const Position& pos) {
	LookatAux aux(pos.n);
	for (auto sc = scenerys.begin(), end = scenerys.end(); sc != end; ++sc) {
		(*sc)->lookat(pos, aux, _base.p, _roll);
	}
	set_posToBase();
	restoreRays_lll();
}

void Camera::takePicture_lll(MlxImage& img) {
	char* data = img.get_data();
	if (data) {
		for (auto pixel = matrix.begin(), end = matrix.end(); pixel != end; ++pixel) {
			memcpy(data, pixel->color.raw, _bytespp);
			data += _bytespp;
		}
	}
}

void Camera::rayTracing_lll(void) {
	for (auto pixel = matrix.begin(), End = matrix.end(); pixel != End; ++pixel) {
		for (auto ray = pixel->rays.begin(), end = pixel->rays.end(); ray != end; ++ray) {
			ray->recursion = 0;
			traceRay(*ray);
		}
		pixel->averageColor();
	}
}

int  Camera::traceRay(Ray& ray) {
	if (ray.recursion > RECURSION_DEPTH) { return 0; }
	A_Scenery* scenery = closestScenery(ray);
	if (!scenery) { ray.color = space ; return space.val; }
	ray.collectLight(scenery->color, ambient);
	ray.changePov();
	scenery->calculateNormal(ray);
	for (auto light = lightsIdx.begin(), end = lightsIdx.end(); light != end; ++light) {
		(*light)->lighting(ray, *scenery, scenerys);
	}
	int _color = ray.color.val;
	if (scenery->reflective > 0) {
		ray.reflect();
		int _reflect = traceRay(ray);
		_color = ray.collectReflect(_color, _reflect, scenery->reflective);
	}
	return _color;
}

A_Scenery* Camera::closestScenery(Ray& ray) {
	A_Scenery*	closestScenery = NULL;
	float		distance = INFINITY;
	for (auto scenery = scenerys.begin(), end = scenerys.end(); scenery != end; ++scenery) {
		if ( (*scenery)->intersection(ray) ) {
			if (distance > ray.dist) {
				distance = ray.dist;
				closestScenery = *scenery;
			}
		}
	}
	if (closestScenery) {
		ray.dist = distance;
	}
	return closestScenery;
}


// Non member functions

std::ostream& operator<<(std::ostream& o, Camera& camera) {
	std::ostringstream os;
	os << std::setw(2) << std::left << camera._nick;
	os << " " << camera._pos.p;
	os << " " << camera._pos.n;
	os << " " << std::setw(4) << camera._fov.get_degree();
	o << std::setw(46) << std::left << os.str();
	o << " #" << camera._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Camera& camera) {
	is >> camera._pos.p >> camera._pos.n >> camera._fov;
	camera._pos.n.normalize();
	return is;
}


// Non member functions
