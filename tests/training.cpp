//
//  training.cpp
//  training
//
//  Created by uru on 08/07/2024.
//

#include "Header.h"

Var	var;

void rt(MlxImage& img) {
	char* 				data = img.get_dataShow();
	int					bytespp = img.get_bytespp();
	int					width = img.get_width();
	int					height = img.get_height();
	Vec2i				mPos; // pixel xy-position on the monitor (width*height pixels, xy(0,0) in the upper left corner, Y-axis direction down);
	Vec2f				cPos; // pixel xy-position on the canvas (width=1, xy(0,0) in the center, XY-axes up and right directions)
	float				mult = 1. / width;
	float				d = d_calculate(90); // argument is horizontal field of view in degrees in range [0,180]
	std::vector<Ray> 	pixels;
	
	pixels.reserve(width * height);
	for (; mPos.y < height; mPos.y++) {
		for (; mPos.x < width; mPos.x++) {
			
			void* pixelAddr = img.get_pixelAddr(data, mPos);
			
			cPos.x = mPos.x; cPos.y = mPos.y;
			cPos = cPos.toRt(width, height) * mult;
			pixels.push_back(Ray(pixelAddr, bytespp, cPos, d));
			memcpy(pixelAddr, img.green.raw, bytespp);
			
//			std::cout << img.get_dataShow() << "\n";
//			std::cout << pixelAddr << "\n";
		}
	}
	
	Sphere	sp1(Vec3f(0,0,20), 10, img.red);
//	Sphere	sp2(Vec3f(2,0,4), 1, img.blue);
//	Sphere	sp3(Vec3f(-2,0,4), 1, img.green);
	
	for (auto it = pixels.begin(); it != pixels.end(); ++it) {
		sp1.intersection(*it);
//		sp2.intersection(*it);
//		sp3.intersection(*it);
		it->drawPixel();
	}

}

int main(void) {
	MlxImage	img;
	
	for (int i = 0; i < 10; i++) {
		std::cout << i << " " << d_calculate(i) << std::endl;
	}
	for (int i = 10; i < 170; i += 10) {
		std::cout << i << " " << d_calculate(i) << std::endl;
	}
	for (int i = 170; i <= 180; i++) {
		std::cout << i << " " << d_calculate(i) << std::endl;
	}
//	std::istringstream	is("sp 1.09877,2,3 5.1 241,242,243 12.2");
//	std::istringstream	is1("1,2,3");
//	std::istringstream	is2("0xF1F2F3");
//	Sphere				sp;
//	Vec3f				v;
//	ARGBColor			c;
//	
//	is >> sp;
//	is1 >> v;
//	is2 >> c;
//	std::cout << sp << std::endl;
//	std::cout << v << std::endl;
//	std::cout << c << std::endl;
//
//	var.img = &img;
//	img.init("Hello!", 800, 600);
//	rt(img);
//	mlx_hook(img.get_win(), ON_DESTROY, 0, destroyNotify, NULL);
//	mlx_hook(img.get_win(), ON_KEYDOWN, 0, keyDown, NULL);
//	mlx_hook(img.get_win(), ON_KEYUP, 0, keyUp, NULL);
//	mlx_hook(img.get_win(), ON_MOUSEDOWN, 0, mouseKeyDown, NULL);
//	mlx_hook(img.get_win(), ON_MOUSEUP, 0, mouseKeyUp, NULL);
//	mlx_hook(img.get_win(), ON_MOUSEMOVE, 0, mouseMove, NULL);
////	mlx_put_image_to_window(img.get_mlx(), img.get_win(), img.get_ptrShowImg(), 0, 0);
//	mlx_loop(img.get_mlx());

	return 0;
}
