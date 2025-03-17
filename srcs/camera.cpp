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
	int sqrSm = sm * sm;
	rays.clear_(sqrSm);
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
			ray->traces.clear_();
			ray->path.clear();
			ray->scnr = NULL;
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
_sm(DAFAULT_SMOOTHING_FACTOR),
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

int Matrix::get_sm(void) { return _sm; }


// class Camera

Camera::Camera(const MlxImage& img) :
Matrix(),
_base(BASE),
_pos(_base),
_roll(0),
_flybyRadius(0),
scenerys(),
objsIdx(),
lightsIdx(),
phMap(),
ambient(),
background(),
depth(DEFAULT_RECURSION_DEPTH),
paths(DAFAULT_PATHS_PER_RAY),
photonMap(NO),
tracingType(RAY),
ambientLightOn(true),
directLightOn(true)
{	_width = img.get_width();
	_height = img.get_height();
	_bytespp = img.get_bytespp();
	_mult = 2. / _width;
}

Camera::~Camera(void) {}

Camera::Camera(const Camera& other) : phMap(other.phMap) { *this = other; }

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
		background = other.background;
		depth = other.depth;
		paths = other.paths;
		photonMap = other.photonMap;
		tracingType = other.tracingType;
		ambientLightOn = other.ambientLightOn;
		directLightOn = other.directLightOn;
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
	paths = DAFAULT_PATHS_PER_RAY;
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
	depth = rd;
	runThreadRoutine(RESTORE_RAYS);
}

void Camera::resetPathsPerRay(int key) {
	paths = pprs[key];
	runThreadRoutine(RESTORE_RAYS);
}

void Camera::changePhotonMap(MapType type) {
	if (photonMap == type) {
		photonMap = NO;
	} else {
		photonMap = type;
	}
	runThreadRoutine(RESTORE_RAYS);
}

void Camera::changeOther(Controls key) {
	switch (key) {
		case AMBIENT_LIGHTING: {
			ambientLightOn = ambientLightOn ? false : true;
			break;
		}
		case DIRECT_LIGHTING: {
			directLightOn = directLightOn ? false : true;
			break;
		}
		case BACKGROUND_LIGHT: {
			if (background.light.val) {
				background = 0;
			} else {
				background = ambient;
				background.set_ratio(ambient.get_ratio() * BACKGRND_VISIBILITY_FACTOR);
			}
			break;
		}
		case RAYTRACING: {
			ambientLightOn = true;
			tracingType = RAY;
			break;
		}
		case PATHTRACING: {
			paths = DAFAULT_PATHS_PER_RAY;
			ambientLightOn = false;
			tracingType = PATH;
			break;
		}
		default:
			break;
	}
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
			pixel->color.gamma(GAMMA);
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
		for (auto ray = pixel->rays.begin(), _end = pixel->rays.end(); ray != _end; ++ray)
			tarcing(*ray);
		pixel->averageColor();
	}
}

void Camera::rayTracing(Camera* camera, size_t begin, size_t end) {
	camera->rayTracing_lll(begin, end);
}

void Camera::tarcing(Ray& ray, int r) {
	if (!rayEnd(ray, r)){
		HitRecord rec(ray.getNormal());
		if (tracingType == RAY) {
			reflections(ray, rec, rec.scnr->reflective, r);
			refractions(ray, rec, rec.scnr->refractive, r);
		} else if (tracingType == PATH) {
			if (ray.path.diffusion()) {
				pathTracing(ray, rec, r);
			} else {
				reflections(ray, rec, rec.scnr->reflective, r);
				refractions(ray, rec, rec.scnr->refractive, r);
				if (ray.path.diffusion())
					pathTracing(ray, rec, r);
			}
		}
		directLightings(ray, rec);
		ambientLighting(ray, rec);
		phMapLightings(ray, rec);
	}
}

bool Camera::rayEnd(Ray& ray, int r) {
	if ((ray.recursion = r) > depth) {
		ray.resetColors();//FIXME
//		ray.paint.reset();
		return true;
	}
	if (!ray.closestScenery(scenerys, _INFINITY)) {
		ray.color.val = background.light.val;//FIXME
		ray.paint = background.light.val;
		return true;
	}
	if (ray.scnr->get_isLight()) {
		ray.color = ray.scnr->get_iColor(ray);//FIXME
		ray.paint = ray.scnr->get_iColor(ray);
		return true;
	}
	ray.markPath();
	return false;
}

void Camera::traceRay(Ray& ray, int r) {
	if (!rayEnd(ray, r)){
		HitRecord rec(ray.getNormal());
		reflections(ray, rec, rec.scnr->reflective, r);
		refractions(ray, rec, rec.scnr->refractive, r);
		directLightings(ray, rec);
		ambientLighting(ray, rec);
		phMapLightings(ray, rec);
	}
}

void Camera::reflections(Ray& ray, const HitRecord& rec, float intensity, int r) {
	if (intensity > _1_255) {
		ColorRecord cRec(ray);
		int   attenuation = rec.scnr->get_iColor(rec);
		ray.reflect();
		traceRay(ray, ++r);
		if (rec.scnr->diffusion)
			attenuation = -1;
		ray.collectReflections(attenuation, intensity, cRec);
		ray.restore(rec);
	}
}

void Camera::refractions(Ray& ray, const HitRecord& rec, float intensity, int r) {
	if (intensity > _1_255) {
		ColorRecord cRec(ray);
		int   attenuation = rec.scnr->get_iColor(rec);
		float schlick = 0;
		if (ray.refract((rec.hit == INSIDE ? rec.scnr->matIOR : rec.scnr->matOIR), schlick)) {
			traceRay(ray, r + 1);
			ray.collectReflections(attenuation, intensity - schlick, cRec);
			ray.restore(rec);
			if (schlick > _1_255)
				reflections(ray, rec, schlick, r);
		} else {
			ray.restore(rec);
			reflections(ray, rec, 1.0, r);
			if (ray.color.val == 0)
				ray.color = background.light;
		}
	}
}

void Camera::pathTracing(Ray& ray, const HitRecord& rec, int r) {
	Average3i	averageColor;
	Average3i	averageShine;
	for (int i = 0; i < paths; i++) {
		ray.resetColors();
		path(ray, rec, r);
		averageColor.add(ray.color);
		averageShine.add(ray.shine);
	}
	averageColor.getARGBColor(ray.color);
	averageShine.getARGBColor(ray.shine);
}

void Camera::tracePath(Ray& ray, int r) {
	if (rayEnd(ray, r))
		return;
	HitRecord	rec(ray.getNormal());
	path(ray, rec, r);
	ambientLighting(ray, rec);
	directLightings(ray, rec);
	phMapLightings(ray, rec);
}

void Camera::path(Ray& ray, const HitRecord& rec, int r) {
	float	mattness = rec.scnr->get_mattness();
	float	attenuation = rec.scnr->get_iColor(rec);
	float	intensity = 1;
	float	shining = -1;
	double	chance = random_double();
	if (chance < rec.scnr->reflective) { /*REFLECTION*/
		ray.reflect(mattness);
		if (rec.scnr->diffusion) attenuation = -1;
	} else if (chance < rec.scnr->reflective + rec.scnr->refractive) { /*REFRACTION*/
		float schlick = mattness >= MATTNESS_GLOSSY_LIMIT ? -1 : 0;
		if (ray.refract(rec.hit == INSIDE ? rec.scnr->matIOR : rec.scnr->matOIR, schlick, mattness)) {
			chance = random_double();
			if (chance > _1_255 && chance < schlick) { /*PARTIAL REFLECTION*/
				ray.restore(rec);
				ray.reflect(mattness);
				if (rec.scnr->diffusion) attenuation = -1;
			}
		} else { /*FULL REFLECTION*/
			ray.reflect(mattness);
			if (rec.scnr->diffusion) attenuation = -1;
		}
	} else { /*DIFFUSION*/
		ray.diffusion();
		intensity = ray.dir * rec.norm;
		shining = shine(rec.dir, rec.norm, ray.dir, rec.scnr->get_glossy());
	}
	tracePath(ray, ++r);
	if (shining == -1)
		ray.collectReflections(attenuation, intensity);
	else
		ray.collectDiffusions(attenuation, intensity, shining);
	ray.restore(rec);
}

void Camera::ambientLighting(Ray& ray, const HitRecord& rec) {
	float diffusion = rec.scnr->diffusion;
	if (ambientLightOn && diffusion) {
		float intensity = -(rec.norm * rec.dir) * diffusion;
		if (intensity > _1_255) {
			ray.light = ambient.light;
			ray.light.attenuate(rec.scnr->get_iColor(rec), intensity);
			ray.color += ray.light;
		}
	}
}

void Camera::directLightings(Ray& ray, const HitRecord& rec) {
	if (directLightOn) {
		float diffusion = rec.scnr->diffusion;
		float glossy = rec.scnr->get_glossy();
		if (diffusion || glossy) {
			for (auto lightSrc = lightsIdx.begin(); lightSrc != lightsIdx.end(); ++lightSrc) {
				float lighting = (*lightSrc)->lighting(ray);
				if (lighting) {
					ray.movePovByNormal(EPSILON);
					if (!ray.closestScenery(scenerys, ray.dist, ANY_SHADOW)) {
						int light = (*lightSrc)->light.light.val;
						if ((lighting *= diffusion) > _1_255) {
							ray.light.val = light;
							ray.color += ray.light.attenuate(rec.scnr->get_iColor(rec), lighting);
						}
						if (glossy) {
							float shining = shine_(rec.dir, rec.norm, ray.dir, glossy);
							if (shining > _1_255) {
								ray.light.val = light;
								ray.shine += ray.light.attenuate(-1, shining);
							}
						}
					}
				}
				ray.restore(rec);
			}
		}
	}
}

void Camera::phMapLightings(Ray& ray, const HitRecord& rec) {
	if (photonMap != NO && rec.scnr->diffusion) {
		phMap.get_traces27(ray.pov, ray.traces, photonMap);
		if (!ray.traces.empty()) {
			ray.phMapLightings(phMap.get_sqr(), phMap.estimate, rec.scnr->get_id(),
							   rec.scnr->get_iColor(rec));
		}
	}
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
