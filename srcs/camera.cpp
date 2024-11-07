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
		_tan = std::tan( degree2radian(_degree / 2) );
		return true;
	}
	return false;
}


// Non member functions

std::ostream& operator<<(std::ostream& o, const Fov& fov) {
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

Pixel::Pixel(const Vec3f& cPos, int sm, float tan, const Vec3f& pos) :
rays(), cPos(cPos), color() {
	reset(sm, tan, pos);
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

void Pixel::reset(int sm, float tan, const Vec3f& pos) {
	rays.clear();
	rays.reserve(sm * sm);
	int sqrSm = sm * sm;
	for (int i = 0; i < sqrSm; i++) {
		rays.push_back(Ray());
	}
	restoreRays(sm, tan, pos);
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
			ray->traces.clear();
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

Fov	Matrix::get_fov(void) { return _fov; }

float Matrix::get_fovDegree(void) { return _fov.get_degree(); }


// class Camera

Camera::Camera(const MlxImage& img) :
_base(BASE),
_pos(_base),
_roll(0),
_flybyRadius(0),
scenerys(),
objsIdx(),
lightsIdx(),
phMap(),
ambient(),
space(),
recursionDepth(RECURSION_DEPTH),
softShadowLength(SOFT_SHADOW_LENGTH_LIMIT),
softShadowSoftness(SOFT_SHADOW_SOFTNESS)
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
		phMap = other.phMap;
		ambient = other.ambient;
		space = other.space;
		recursionDepth = other.recursionDepth;
		softShadowLength = other.softShadowLength;
		softShadowSoftness = other.softShadowSoftness;
	}
	return *this;
}

Position Camera::get_pos(void) const { return _pos; }

float Camera::get_rollDegree(void) const { return radian2degree(_roll); }

float Camera::get_roll(void) const { return _roll; }

float Camera::get_flybyRadius(void) const { return _flybyRadius; }

void Camera::set_scenery(A_Scenery* scenery) {
	scenerys.push_back(scenery);
	if ( scenerys.back()->get_isLight() == true ) {
		lightsIdx.push_back(scenery);
	} else {
		objsIdx.push_back(scenery);
	}
}

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

void Camera::restoreRays(Camera* camera, size_t begin, size_t end) {
	camera->restoreRays_lll(begin, end);
}

void Camera::resetRays(Camera* camera, size_t begin, size_t end) {
	camera->resetRays_lll(begin, end);
}

void Camera::restoreRays_lll(size_t begin, size_t end) {
	float tan = _fov.get_tan();
	for (auto pixel = matrix.begin() + begin, _end = matrix.begin() + end; pixel != _end; ++pixel) {
		pixel->restoreRays(_sm, tan, _pos.p);
	}
}

void Camera::resetRays_lll(size_t begin, size_t end) {
	float sm_mult = (1. / _sm) * _mult;
	float tan = _fov.get_tan();
	for (auto pixel = matrix.begin() + begin, _end = matrix.begin() + end; pixel != _end; ++pixel) {
		pixel->cPos.z = sm_mult;
		pixel->reset(_sm, tan, _pos.p);
	}
}

bool Camera::resetFovDegree(float degree) {
	if (_fov.set_degree(degree)) {
		runThreadRoutine(RESTORE_RAYS);
		return true;
	}
	return false;
}

void Camera::resetSmoothingFactor(int sm) {
	_sm = sm;
	runThreadRoutine(RESET_RAYS);
}

void Camera::resetRecursionDepth(int rd) {
	recursionDepth = rd;
	runThreadRoutine(RESTORE_RAYS);
}

void Camera::resetSoftShadowLength(float sl) {
	softShadowLength = sl;
	runThreadRoutine(RESTORE_RAYS);
}

void Camera::resetSoftShadowSoftness(float ss) {
	softShadowSoftness = ss;
	runThreadRoutine(RESTORE_RAYS);
}

void Camera::resetRoll(float roll) {
	if (roll >= 90.) {
		roll = degree2radian(90.);
	} else if (roll <= -90.) {
		roll = degree2radian(-90.);
	} else if ( almostEqual(roll, 0., EPSILON) ) {
		roll = degree2radian(0.);
	} else {
		roll = degree2radian(roll);
	}
	float shiftRoll = roll - _roll;
	_roll = roll;
	for (auto sc = scenerys.begin(), end = scenerys.end(); sc != end; ++sc) {
		(*sc)->roll(_pos.p, shiftRoll);
	}
	runThreadRoutine(RESTORE_RAYS);
	if (DEBUG_MODE) { std::cout << "roll: " << radian2degree(_roll) << std::endl; }
}

void Camera::lookatCamera(const Position& pos) {
	LookatAux aux(pos.n);
	for (auto sc = scenerys.begin(), end = scenerys.end(); sc != end; ++sc) {
		(*sc)->lookat(pos, aux, _base.p, _roll);
	}
	phMap.lookat(pos, aux, _roll);
	set_posToBase();
	runThreadRoutine(RESTORE_RAYS);
}

void Camera::takePicture_lll(MlxImage& img, size_t begin, size_t end) {
	char* data = img.get_data();
	if (data) {
		data += _bytespp * begin;
		for (auto pixel = matrix.begin() + begin, _end = matrix.begin() + end; pixel != _end; ++pixel) {
			memcpy(data, pixel->color.raw, _bytespp);
			data += _bytespp;
		}
	}
}

void Camera::takePicture(Camera* camera, MlxImage& img, size_t begin, size_t end) {
	camera->takePicture_lll(img, begin, end);
}

void Camera::rayTracing_lll(size_t begin, size_t end) {
	for (auto pixel = matrix.begin() + begin, End = matrix.begin() + end; pixel != End; ++pixel) {
		for (auto ray = pixel->rays.begin(), _end = pixel->rays.end(); ray != _end; ++ray) {
			traceRay(*ray);
		}
		pixel->averageColor();
	}
}

void Camera::rayTracing(Camera* camera, size_t begin, size_t end) {
	camera->rayTracing_lll(begin, end);
}

void Camera::traceRay(Ray& ray, int r) {
	if (r > recursionDepth) { return; }
	ray.recursion = r;
	A_Scenery* scenery = ray.closestScenery(scenerys, _INFINITY);
	if (!scenery) { ray.color = space ; return; }
	ray.fixDirFromCam_if();
	scenery->giveNormal(ray);
	ray.movePovByNormal(EPSILON);
	caustics(ray, *scenery);
	lightings(ray, *scenery, r);	// order matters
	reflections(ray, *scenery, r);	// order matters
	refractions(ray, *scenery, r);	// order matters
}

void Camera::lightings(Ray& ray, const A_Scenery& scenery, int r) {
	(void)r;
	ray.light = ambient;
	ray.collectLight(scenery.color);
	for (auto light = lightsIdx.begin(), end = lightsIdx.end(); light != end; ++light) {
		float k = (*light)->lighting(ray);
		if (k) {
			shadow_if(ray, scenery, k, r);
		}
	}
}

void Camera::caustics(Ray& ray, const A_Scenery& scenery) {
	phMap.get_traces27(ray.pov, ray.traces, CAUSTIC);
	ray.phMaplightings();
	ray.collectLight(scenery.color);
	
}

void Camera::reflections(Ray& ray, const A_Scenery& scenery, int r) {
	if (scenery.reflective) {
		if (scenery.refractive) {
			return;
		}
		int _color = ray.color.val, _shine = ray.shine.val;
		ray.color = ray.shine = 0;
		if (scenery.refractive) {
			RayBasic raySafe(ray);
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
//			ray.collectLight(scenery.color);
		}
	}
}

void Camera::shadow_if(Ray& ray, const A_Scenery& scenery, float k, int r) {
	(void)r;
	Hit		hit = ray.hit;
	float	distToLight = ray.dist, d = 1.;
	A_Scenery* shader = ray.closestScenery(scenerys, distToLight, FIRST_SHADOW);
	if (softShadowLength < SOFT_SHADOW_LENGTH_LIMIT) {
		d = softShadowMultiplier(ray, distToLight);
	} else if (shader) {
		d = 0.;
	}
//	if (shader && shader->refractive) {
//		transparentShadow(ray, *shader, d, r);
//	} else {
//		ray.light.product(d);
//	}
	ray.light.product(d);

	ray.collectLight(scenery.color, k);
	ray.collectShine(scenery.specular, d);
	ray.hit = hit;
}

float Camera::softShadowMultiplier(Ray& ray, float distToLight) {
	float	d = 1., previous = 1.;
	if (!ray.segments.empty()) {
		ray.segments.sort();
		for (auto segment = ray.segments.begin(), end = ray.segments.end(); segment != end; ++segment) {
			if (segment->b.d == ray.dist) {	// pos is inside the shader
				d = 0.;
				break;
			}
			if (segment->b.d > 0 && segment->b.d < distToLight) {
				float _d = segment->b.s->getDistanceToShaderEdge(ray, segment->b.d, segment->b.inside);
				d *= softShadow(_d, segment->b.d - previous, softShadowLength, softShadowSoftness);
				previous = segment->b.d;
			}
		}
	}
	return d;
}

bool Camera::transparentShadow(Ray& ray, const A_Scenery& shader, float d, int r) {
	RayBasic	raySafe(ray);
	ray.dir = ray.dirL;
	shader.giveNormal(ray);
	if (ray.dir.refract(ray.norm, ray.hit == INSIDE ? shader.matIOR : shader.matOIR)) {
		ColorsSafe	colorsSafe;
		ray.getColorsSafe(colorsSafe);
		ray.movePovByNormal(EPSILON);
		traceRay(ray, ++r);
		ray.collectRefractiveLight(shader.color, colorsSafe.color, shader.refractive);
		ray.collectShadowLight(colorsSafe, d);
		ray.restore(raySafe);
		return true;
	}
	ray.restore(raySafe);
	return false;
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
	}
}

void Camera::runThreadRoutine(int routine, MlxImage* img) {
	size_t size = this->matrix.size();
	size_t begin, end;
	int numThreads = (int)(size / PIXELS_PER_THREAD);
	if (size % PIXELS_PER_THREAD > 0) {
		numThreads++;
	}
	std::thread th[numThreads];
	for (int i = 0; i < numThreads; i++) {
		begin = i * size / numThreads;
		if (i == numThreads - 1) {
			end = size;
		} else {
			end = size / numThreads * (i + 1);
		}
		switch (routine) {
			case RESTORE_RAYS:
				th[i] = std::thread([this, begin, end](){restoreRays(this, begin, end);});
				break ;
			case RESET_RAYS:
				th[i] = std::thread([this, begin, end](){resetRays(this, begin, end);});
				break ;
			case TAKE_PICTURE:
				if (img != NULL) {
					th[i] = std::thread([this, img, begin, end](){takePicture(this, *img, begin, end);});
				}
				break ;
			case RAY_TRACING:
				th[i] = std::thread([this, begin, end](){rayTracing(this, begin, end);});
				break ;
		}
	}
	for (int i = 0; i < numThreads; i++) {
		th[i].join();
	}

}


// Non member functions

std::ostream& operator<<(std::ostream& o, const Camera& camera) {
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

