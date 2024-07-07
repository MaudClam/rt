//
//  main.cpp
//  rt
//
//  Created by uru on 05/07/2024.
//

#include "Header.h"

Var	var;

int main(void) {
	MlxImage	img;
	
	var.img = &img;
	img.init("Hello!", 800, 600);
	mlx_hook(img.get_win(), ON_DESTROY, 0, destroyNotify, NULL);
	mlx_hook(img.get_win(), ON_KEYDOWN, 0, keyDown, NULL);
	mlx_hook(img.get_win(), ON_KEYUP, 0, keyUp, NULL);
	mlx_hook(img.get_win(), ON_MOUSEDOWN, 0, mouseKeyDown, NULL);
	mlx_hook(img.get_win(), ON_MOUSEUP, 0, mouseKeyUp, NULL);
	mlx_hook(img.get_win(), ON_MOUSEMOVE, 0, mouseMove, NULL);
	mlx_loop(img.get_mlx());

	return 0;
}
