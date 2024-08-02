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

Pixel::Pixel(const Vec2f& cPos, float tan, const Position& pos) : ray(), cPos(cPos) {
	reset(tan, pos);
}

Pixel::~Pixel(void) {}

Pixel::Pixel(const Pixel& other) :
ray(other.ray),
cPos(other.cPos)
{}

Pixel& Pixel::operator=(const Pixel& other) {
	if (this != &other) {
		ray = other.ray;
		cPos = other.cPos;
	}
	return *this;
}

void Pixel::reset(float tan, const Position& pos) {
	ray.pov = pos.p;
	ray.dir.x = cPos.x * tan; ray.dir.y = cPos.y * tan; ray.dir.z = 1;
	ray.dir.normalize();
	ray.dist = INFINITY;
}


// class Matirx

Matrix::Matrix(void) :
_width(0),
_height(0),
_bytespp(0),
_mult(0),
_fov(),
matrix()
{}

Matrix::~Matrix(void) {}

Matrix::Matrix(const Matrix& other) :
_width(other._width),
_height(other._height),
_bytespp(other._bytespp),
_mult(other._mult),
_fov(other._fov),
matrix(other.matrix)
{}

Matrix& Matrix::operator=(const Matrix& other) {
	if (this != &other) {
		_width = other._width;
		_height = other._height;
		_bytespp = other._bytespp;
		_mult = other._mult;
		_fov = other._fov;
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

bool Matrix::set_fovDegree(float degree) { return _fov.set_degree(degree); }


// class Camera

Camera::Camera(const MlxImage& img) :
_pos(Vec3f(), Vec3f(0,0,1)),
_roll(0),
_flybyRadius(0)
{	_width = img.get_width();
	_height = img.get_height();
	_bytespp = img.get_bytespp();
	_mult = 2. / _width;
	initMatrix();
}

Camera::~Camera(void) {}

Camera::Camera(const Camera& other) { *this = other; }

Camera& Camera::operator=(const Camera& other) {
	if (this != &other) {
		_width = other._width;
		_height = other._height;
		_bytespp = other._bytespp;
		_mult = other._mult;
		_fov = other._fov;
		matrix = other.matrix;
		_pos = other._pos;
		_roll = other._roll;
		_flybyRadius = other._flybyRadius;
	}
	return *this;
}

Position Camera::get_pos(void) const { return _pos; }

float Camera::get_rollDegree(void) const { return degree(_roll); }

float Camera::get_roll(void) const { return _roll; }

float Camera::get_flybyRadius(void) const { return _flybyRadius; }

void Camera::set_pos(const Position& pos) { _pos = pos; }

void Camera::set_flybyRadius(float flybyRadius) { _flybyRadius = flybyRadius; }

void Camera::initMatrix(void) {
	Vec2i	mPos; // pixel xy-coordinate on the monitor (width*height pixels, xy(0,0) in the upper left corner, Y-axis direction down);
	Vec2f	cPos; // pixel xy-coordinate on the canvas (width=1, xy(0,0) in the center, XY-axes up and right directions)
	matrix.reserve(_width * _height);
	for (mPos.y = 0; mPos.y < _height; mPos.y++) {
		for (mPos.x = 0; mPos.x < _width; mPos.x++) {
			cPos.x = mPos.x; cPos.y = mPos.y;
			cPos = cPos.toRt(_width, _height) * _mult;
			matrix.push_back( Pixel(cPos, _fov.get_tan(), _pos) );
		}
	}
}

void Camera::resetMatrix(void) {
	auto End = matrix.end();
	for (auto pixel = matrix.begin(); pixel != End; ++pixel) {
		pixel->reset(_fov.get_tan(), _pos);
	}
}

void Camera::takePicture(MlxImage& img) {
	char* data = img.get_data();
	if (data) {
		auto End = matrix.end();
		for (auto pixel = matrix.begin(); pixel != End; ++pixel) {
			memcpy(data, pixel->ray.color.raw, _bytespp);
			data += _bytespp;
			pixel->ray.color.val = 0;
		}
	}
}

bool Camera::reset_fovDegree(float degree) {
	if (_fov.set_degree(degree)) {
		resetMatrix();
		return true;
	}
	return false;
}

void Camera::reset_pos(const Position& pos) {
	_pos = pos;
	resetMatrix();
}

void Camera::reset_roll(float roll) {
	if (roll >= 90) {
		_roll = radian(90);
	} else if (roll <= -90) {
		_roll = radian(-90);
	} else if (roll == 0) {
		_roll = radian(0);
	} else {
		_roll = radian(roll);
	}
	if (DEBUG) { std::cout << "roll: " << degree(_roll) << std::endl; }
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
	camera.resetMatrix();
	return is;
}


// Non member functions
