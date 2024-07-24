//
//  training.cpp
//  training
//
//  Created by uru on 08/07/2024.
//

#include "Header.h"

Var	var;

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
