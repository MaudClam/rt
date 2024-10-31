//
//  main.cpp
//  rt
//
//  Created by uru on 05/07/2024.
//

#include "MlxImage.hpp"

Var	var;

int main(int ac, char** av) {
	MlxImage	img;
	Scene		scene(img);
	var.img		= &img;
	var.scene	= &scene;
	system("clear");
	scene.mesage(WELLCOM_MSG);
	outputFile(CONTROLS_FILE);
	if (scene.parsing(ac, av))
		return ERROR;
	void* mlx = img.get_mlx();
	scene.rt();
	mlx_loop_hook(mlx, flyby, NULL);
	mlx_loop(mlx);
	return SUCCESS;
}
