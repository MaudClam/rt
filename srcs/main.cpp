//
//  main.cpp
//  rt
//
//  Created by uru on 05/07/2024.
//

# include "Header.h"

Var	var;

int main(int ac, char** av) {
	MlxImage	img;
	Scene		scene(img);
	var.img		= &img;
	var.scene	= &scene;
	if (DEBUG_MODE) {
		std::cout << ON_KEYDOWN << std::endl;
	}
	int error	= scene.parsing(ac, av);
	if (error != SUCCESS) {
		return error;
	}
	scene.rt();
	mlx_loop_hook(img.get_mlx(), flyby, NULL);
	mlx_loop(img.get_mlx());
	return SUCCESS;
}
