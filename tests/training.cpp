//
//  training.cpp
//  training
//
//  Created by uru on 08/07/2024.
//

#include "Header.h"

Var	var;

void rt(MlxImage& img, Scene& scene) {
	char* 				data = img.get_data();
	int					bytespp = img.get_bytespp();
	int					width = img.get_width();
	int					height = img.get_height();
	Vec2i				mPos; // pixel xy-position on the monitor (width*height pixels, xy(0,0) in the upper left corner, Y-axis direction down);
	Vec2f				cPos; // pixel xy-position on the canvas (width=1, xy(0,0) in the center, XY-axes up and right directions)
	float				mult = 2. / width;
	float				tan = std::tan( (60 / 2) * (std::numbers::pi / 180) ); // argument is horizontal field of view in degrees in range [0,180]
	std::vector<Ray_> 	pixels;
	
	pixels.reserve(width * height);
	for (; mPos.y < height; mPos.y++) {
		for (mPos.x = 0; mPos.x < width; mPos.x++) {
			
			cPos.x = mPos.x; cPos.y = mPos.y;
			cPos = cPos.toRt(width, height) * mult;
			pixels.push_back(Ray_(data, bytespp, cPos, tan));
			data += bytespp;
		}
	}
	for (auto pixel = pixels.begin(); pixel != pixels.end(); ++pixel) {
		for (auto obj = scene.objsIdx.begin(); obj != scene.objsIdx.end(); ++obj) {
			if ( (*obj)->intersection(*pixel) && pixel->dist < INFINITY) {
				pixel->color = (*obj)->color;
			}
		}
	}
	for (auto pixel = pixels.begin(); pixel != pixels.end(); ++pixel) {
		pixel->drawPixel();
	}
}

int main(int ac, char** av) {
	MlxImage	img;
	Scene		scene(img);
	var.img		= &img;
	var.scene	= &scene;

	if (scene.parsing(ac, av) != SUCCESS) {
		return ERROR;
	}
	scene.rt();
	mlx_loop(img.get_mlx());
	return SUCCESS;
}
