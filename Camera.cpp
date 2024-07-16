//
//  Camera.cpp
//  rt
//
//  Created by uru on 13/07/2024.
//

#include "Camera.hpp"

Camera::Camera(const MlxImage& img) :
data(img.get_data()),
width(img.get_width()),
height(img.get_height()),
bytespp(img.get_bytespp()),
mult(2. / width),
fov(60.),
tan(fovToTan(this->fov)),
pos(Vec3f(0,0,0),Vec3f(0,0,1)),
pixels()
{}

Camera::~Camera(void) {}

Camera::Camera(const MlxImage& img, const Position& pos, float fov) :
data(img.get_data()),
width(img.get_width()),
height(img.get_height()),
bytespp(img.get_bytespp()),
mult(2. / width),
fov(fov),
tan(fovToTan(this->fov)),
pos(pos),
pixels()
{ this->pos.n.normalize(); }

Camera::Camera(const Camera& other) :
data(other.data),
width(other.width),
height(other.height),
bytespp(other.bytespp),
mult(other.mult),
fov(other.fov),
tan(other.tan),
pos(other.pos),
pixels(other.pixels)
{}

Camera& Camera::operator=(const Camera& other) {
	if (this != &other) {
		data = other.data;
		width = other.width;
		height = other.height;
		bytespp = other.bytespp;
		mult = other.mult;
		fov = other.fov;
		tan = other.tan;
		pos = other.pos;
		pixels = other.pixels;
	}
	return *this;
}

float Camera::get_fov(void) const { return fov; }

Position Camera::get_pos(void) const { return pos; }

bool Camera::set_fov(float fov) {
	this->fov = fov;
	this->tan = fovToTan(this->fov);
	return this->fov == fov;
}

void Camera::set_pos(const Position& pos) {
	this->pos = pos;
}

void Camera::initPixels(void) {
	Vec2i	mPos; // pixel xy-position on the monitor (width*height pixels, xy(0,0) in the upper left corner, Y-axis direction down);
	Vec2f	cPos; // pixel xy-position on the canvas (width=1, xy(0,0) in the center, XY-axes up and right directions)

	pixels.reserve(width * height);
	for (mPos.y = 0; mPos.y < height; mPos.y++) {
		for (mPos.x = 0; mPos.x < width; mPos.x++) {
			cPos.x = mPos.x; cPos.y = mPos.y;
			cPos = cPos.toRt(width, height) * mult;
			pixels.push_back(Ray(data, bytespp, cPos, tan));
			data += bytespp;
		}
	}
}

void Camera::resetPixels(void) {
	for (auto pixel = pixels.begin(); pixel != pixels.end(); ++pixel) {
		pixel->reset(tan);
	}
}


// Non member functions

std::ostream& operator<<(std::ostream& o, Camera& camera) {
	o	<< camera.nick
		<< " " << camera.pos.p
		<< " " << camera.pos.n
		<< " " << camera.fov
		<< "\t#" << camera.name;
	return o;
}

std::istringstream& operator>>(std::istringstream& is, Camera& camera) {
	if (!is.str().compare(0, camera.nick.size(), camera.nick)) {
		char trash;
		for (size_t i = 0; i < camera.nick.size(); ++i) {
			is >> trash;
		}
		is >> camera.pos.p >> camera.pos.n >> camera.fov;
		camera.pos.n.normalize();
		camera.tan = fovToTan(camera.fov);
	}
	return is;
}

float fovToTan(float& fov) {
	fov = (fov < 0 ? 0 : fov);
	fov = (fov > 180 ? 180 : fov);
	return std::tan( (fov / 2.) * (std::numbers::pi / 180.) );
}

float tanToFov(float tan) {
	return 360. * std::atan(tan) / std::numbers::pi;
}
