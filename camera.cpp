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


// struct Ray

Ray::Ray(void) : pov(), dir(), dist(0), touch(0), color() {}

Ray::~Ray(void) {}

Ray::Ray(const Ray& other) :
pov(other.pov),
dir(other.dir),
dist(other.dist),
touch(other.touch),
color(other.color)
{}

Ray& Ray::operator=(const Ray& other) {
	if (this != & other) {
		pov = other.pov;
		dir = other.dir;
		dist = other.dist;
		touch = other.touch;
	}
	return *this;
}


// struct Pixel

Pixel::Pixel(const Vec2f& cPos, float tan, const Vec3f& pov) : ray(), cPos(cPos) {
	reset(tan, pov);
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

void Pixel::reset(float tan, const Vec3f& pov) {
	ray.pov = pov;
	ray.dir.x = cPos.x * tan; ray.dir.y = cPos.y * tan; ray.dir.z = 1.;
	ray.dir.normalize();
	ray.dist = INFINITY;
	ray.touch = 0;
	ray.color.val = 0; ray.color.bytespp = ARGB;
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

void Matrix::set_width(int width) { this->_width = width; _mult = 2. / 2; }

void Matrix::set_height(int height) { this->_height = height; }

void Matrix::set_bytespp(int bytespp) { this->_bytespp = bytespp; }

bool Matrix::set_fovTan(float tan) { return _fov.set_tan(tan); }

bool Matrix::set_fovDegree(float degree) { return _fov.set_degree(degree); }


// class Camera

Camera::Camera(const MlxImage& img) :
_pos(BASE),
_pos0(BASE),
_roll(0),
_flybyRadius(0)
{	_width = img.get_width();
	_height = img.get_height();
	_bytespp = img.get_bytespp();
	_mult = 2. / _width;
	initMatrix();
}

Camera::~Camera(void) {}

Camera::Camera(const MlxImage& img, const Position& pos, float fov) :
_pos0(BASE)
{
	_width = img.get_width();
	_height = img.get_height();
	_bytespp = img.get_bytespp();
	_mult = 2. / _width;
	set_fovDegree(fov);
	_pos = pos;
	_pos.n.normalize();
	_roll = 0;
	_flybyRadius = 0;
	initMatrix();
}

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
		_pos0 = other._pos0;
		_roll = other._roll;
		_flybyRadius = other._flybyRadius;
	}
	return *this;
}

Position Camera::get_pos(void) const { return _pos; }

Position Camera::get_pos0(void) const { return _pos0; }

float Camera::get_roll(void) const { return radian(_roll); }

float Camera::get_flybyRadius(void) const { return _flybyRadius; }

void Camera::set_pos(const Position& pos) { this->_pos = pos; }

void Camera::set_pos0(const Position& pos0) { this->_pos0 = pos0; }

void Camera::set_roll(float roll) {
	if (roll > 90 || almostEqual(roll, 90, EPSILON)) {
		this->_roll = 90 - EPSILON;
	} else if (roll < -90 || almostEqual(roll, 90, EPSILON)) {
		this->_roll = -90 + EPSILON;
	} else if (almostEqual(roll, 0, 1)) {
		this->_roll = 0;
	} else {
		this->_roll = roll;
	}
}

void Camera::set_flybyRadius(float flybyRadius) { this->_flybyRadius = flybyRadius; }

void Camera::initMatrix(void) {
	Vec2i	mPos; // pixel xy-position on the monitor (width*height pixels, xy(0,0) in the upper left corner, Y-axis direction down);
	Vec2f	cPos; // pixel xy-position on the canvas (width=1, xy(0,0) in the center, XY-axes up and right directions)
	matrix.reserve(_width * _height);
	for (mPos.y = 0; mPos.y < _height; mPos.y++) {
		for (mPos.x = 0; mPos.x < _width; mPos.x++) {
			cPos.x = mPos.x; cPos.y = mPos.y;
			cPos = cPos.toRt(_width, _height) * _mult;
			matrix.push_back( Pixel(cPos, _fov.get_tan(), _pos0.p) );
		}
	}
}

void Camera::resetMatrix(void) {
	for (auto pixel = matrix.begin(); pixel != matrix.end(); ++pixel) {
		pixel->reset(_fov.get_tan(), _pos0.p);
	}
}

void Camera::takePicture(MlxImage& img) {
	char* data = img.get_data();
	if (data) {
		for (auto pixel = matrix.begin(); pixel != matrix.end(); ++pixel) {
			memcpy(data, pixel->ray.color, _bytespp);
			data += _bytespp;
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

bool Camera::reset_pov(const Vec3f& pov) {
	if (pov != _pos0.p) {
		_pos0.p = pov;
		for (auto pixel = matrix.begin(); pixel != matrix.end(); ++pixel) {
			pixel->ray.pov = _pos0.p;
			pixel->ray.color.val = 0; pixel->ray.color.bytespp = ARGB;
		}
		return true;
	}
	return false;
}


bool Camera::move(int ctrl) {
	switch (ctrl) {
		case MOVE_RIGHT: {
			if ( reset_pov( Vec3f(STEP_MOVE,0,0)) ) { return true; }
			break;
		}
		case MOVE_LEFT: {
			if ( reset_pov( Vec3f(-STEP_MOVE,0,0)) ) { return true; }
			break;
		}
		case MOVE_UP: {
			if ( reset_pov( Vec3f(0,STEP_MOVE,0)) ) { return true; }
			break;
		}
		case MOVE_DOWN: {
			if ( reset_pov( Vec3f(0,-STEP_MOVE,0)) ) { return true; }
			break;
		}
		case MOVE_FORWARD: {
			if ( reset_pov( Vec3f(0,0,STEP_MOVE)) ) { return true; }
			break;
		}
		case MOVE_BACKWARD: {
			if ( reset_pov( Vec3f(0,0,-STEP_MOVE)) ) { return true; }
			break;
		}
		default:
			break;
	}
	return false;
}

//void Camera::flyby(int ctrl) {
//	float x = 0, z = 0;
//	switch (ctrl) {
//		case MOVE_RIGHT: {
//			x = std::sin(radian(-STEP_MOVE)); z = flybyRadius - std::sqrt(x * x + flybyRadius * flybyRadius);
//			pos.lookatBase( Position(Vec3f(x,0,z),Vec3f(-x,0,flybyRadius - z).normalize()), get_roll() );
//			break;
//		}
//		case MOVE_LEFT: {
//			x = STEP_MOVE; z = flybyRadius - std::sqrt(x * x + flybyRadius * flybyRadius);
//			pos.lookatBase( Position(Vec3f(x,0,z),Vec3f(-x,0,flybyRadius - z).normalize()), get_roll() );
//			break;
//		}
//		case MOVE_UP: {
//			pos.lookatBase( Position(Vec3f(0,STEP_MOVE,0),BASE_DIR), get_roll() );
//			break;
//		}
//		case MOVE_DOWN: {
//			pos.lookatBase( Position(Vec3f(0,-STEP_MOVE,0),BASE_DIR), get_roll() );
//			break;
//		}
//		case MOVE_FORWARD: {
//			flybyRadius -= STEP_MOVE;
//			pos.lookatBase( Position(Vec3f(0,0,STEP_MOVE),BASE_DIR), get_roll() );
//			break;
//		}
//		case MOVE_BACKWARD: {
//			flybyRadius += STEP_MOVE;
//			pos.lookatBase( Position(Vec3f(0,0,-STEP_MOVE),BASE_DIR), get_roll() );
//			break;
//		}
//		default:
//			break;
//	}
//}

//void Camera::rotate(int ctrl) {
//	float x = 0, y = 0, z = 0;
//	switch (ctrl) {
//		case YAW_RIGHT: {
//			z = std::cos(radian(STEP_ROTATION));
//			x = std::sin(radian(STEP_ROTATION));
//			pos.lookatBase( Position(Vec3f(),Vec3f(x,0,z)), get_roll() );
//			break;
//		}
//		case YAW_LEFT: {
//			z = std::cos(radian(-STEP_ROTATION));
//			x = std::sin(radian(-STEP_ROTATION));
//			pos.lookatBase( Position(Vec3f(),Vec3f(x,0,z)), get_roll() );
//			break;
//		}
//		case PITCH_UP: {
//			z = std::cos(radian(STEP_ROTATION));
//			y = std::sin(radian(STEP_ROTATION));
//			pos.lookatBase( Position(Vec3f(),Vec3f(0,y,z)), get_roll() );
//			break;
//		}
//		case PITCH_DOWN: {
//			z = std::cos(radian(-STEP_ROTATION));
//			y = std::sin(radian(-STEP_ROTATION));
//			pos.lookatBase( Position(Vec3f(),Vec3f(0,y,z)), get_roll() );
//			break;
//		}
//		case ROLL_RIGHT: {
//			set_roll(roll - STEP_ROTATION);
//			pos.lookatBase( Position(Vec3f(),BASE_DIR), get_roll() );
//			break;
//		}
//		case ROLL_LEFT: {
//			set_roll(roll + STEP_ROTATION);
//			pos.lookatBase( Position(Vec3f(),BASE_DIR), get_roll() );
//			break;
//		}
//		default:
//			break;
//	}
//	std::cout << "roll: " << roll << std::endl;
//}


// Non member functions

std::ostream& operator<<(std::ostream& o, Camera& camera) {
	o	<< camera._nick
		<< " " << camera._pos.p
		<< " " << camera._pos.n
		<< " " << camera._fov.get_degree()
		<< "\t#" << camera._name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Camera& camera) {
	if (!is.str().compare(0, camera._nick.size(), camera._nick)) {
		char trash;
		for (size_t i = 0; i < camera._nick.size(); ++i) {
			is >> trash;
		}
		is >> camera._pos.p >> camera._pos.n >> camera._fov;
		camera._pos.n.normalize();
	}
	camera.resetMatrix();
	return is;
}


// Non member functions
