#include "MlxImage.hpp"

Var	var;

int main(int ac, char** av) {
	float x = 1.9; int y = 2;
	std::cout << std::remainder(x, y) << std::endl;
	std::cout << std::fmod(x, y) << std::endl;
	int x1 = 33456, y1 = 20000;
	std::cout << x1 % y1 << std::endl;

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
