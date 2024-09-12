//
//  main.cpp
//  rt
//
//  Created by uru on 05/07/2024.
//

#include "Header.h"

Var	var;

int main(int ac, char** av) {
	system("clear");
	outputFile(CONTROLS_FILE);
	MlxImage	img;
	Scene		scene(img);
	var.img		= &img;
	var.scene	= &scene;
	int error	= scene.parsing(ac, av);
	if (error != SUCCESS)
		return error;
	void* mlx = img.get_mlx();
	scene.rt();
	mlx_loop_hook(mlx, flyby, NULL);
	mlx_loop(mlx);
	return SUCCESS;
}
