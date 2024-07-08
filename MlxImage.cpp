//
//  MlxImage.cpp
//  rt
//
//  Created by uru on 06/07/2024.
//

#include "MlxImage.hpp"

extern Var var;

MlxImage::MlxImage() :
mlx(),
win(),
ptrDrawImg(),
ptrShowImg(),
dataDraw(),
dataShow(),
pointers(),
width(0),
height(0),
bytespp(0),
lineLen(0),
endian(0),
holdKey(0),
mouseHoldKey(0)
{}

MlxImage::~MlxImage(void) {
	if (DEBUG_MODE) { std::cout << "~MlxImage() destructor was done.\n"; }
}

void*	MlxImage::get_mlx(void) const { return mlx; }

void*	MlxImage::get_win(void) const { return win; }

void*	MlxImage::get_ptrDrawImg(void) const { return ptrDrawImg; }

void*	MlxImage::get_ptrShowImg(void) const { return ptrShowImg; }

char*	MlxImage::get_dataDraw(void) const { return dataDraw; }

char*	MlxImage::get_dataShow(void) const { return dataShow; }

int		MlxImage::get_width(void) const { return width; }

int		MlxImage::get_height(void) const { return height; }

int		MlxImage::get_bytespp(void) const { return bytespp; }

int		MlxImage::get_lineLen(void) const { return lineLen; }

int		MlxImage::get_endian(void) const { return endian; }

char*	MlxImage::get_pixelAddr(char* data, int x, int y, Y_direction yd) const {
	if (!data || x < 0 || y < 0 || x >= width || y >= height)
		return NULL;
	if (yd == DOWN)
		return data + y * width * bytespp + x * bytespp;
	return data + (x + (height - y - 1) * width) * bytespp;
}

void	MlxImage::swap(void){
	std::swap(ptrDrawImg, ptrShowImg);
	std::swap(dataDraw, dataShow);
}

void	MlxImage::fill(char* data, const ARGBColor& color) {
	char* addr = NULL;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			addr = get_pixelAddr(data, x, y);
			memcpy(addr, color.raw, bytespp);
		}
	}
}

void	MlxImage::clear(const ARGBColor& color, ClearWhat target) {
	if (target == BOTH) {
		fill(dataDraw, color);
		fill(dataShow, color);
	} else if (target == DRAW_IMG) {
		fill(dataDraw, color);
	} else if (target == SHOW_IMG) {
		fill(dataShow, color);
	}
}

void	MlxImage::init(const std::string& header, int w, int h) {
	width = w;
	height = h;

	if ( !(mlx = mlx_init()) ) {
		int _errno = errno;
		std::cerr << "Error mlx_init(): " << strerror(_errno) << std::endl;
		exit(_errno) ;
	}
	pointers.push(mlx);

	if ( !(win = mlx_new_window(mlx, width, height, (char*)header.c_str())) ) {
		int _errno = errno;
		freePointers();
		std::cerr << "Error mlx_new_window(): " << strerror(_errno) << std::endl;
		exit(_errno) ;
	}
	pointers.push(win);

	if ( !(ptrDrawImg = mlx_new_image(mlx, width, height)) ) {
		int _errno = errno;
		freePointers();
		std::cerr << "Error mlx_new_image(): " << strerror(_errno) << std::endl;
		exit(_errno) ;
	}
	pointers.push(ptrDrawImg);

	int bitsPerPixel = 0;
	dataDraw = mlx_get_data_addr(ptrDrawImg, &bitsPerPixel, &lineLen, &endian);
	if (bitsPerPixel % 8 != 0) {
		std::cerr	<< "Error mlx_get_data_addr(): gives the invalid bits_per_pixel parameter '"
					<< bitsPerPixel << "'" << std::endl;
	}
	bytespp = bitsPerPixel / 8;

	if ( !(ptrShowImg = mlx_new_image(mlx, width, height)) ) {
		int _errno = errno;
		freePointers();
		std::cerr << "Error mlx_new_image(): " << strerror(_errno) << std::endl;
		exit(_errno) ;
	}
	pointers.push(ptrShowImg);

	dataShow = mlx_get_data_addr(ptrShowImg, &bitsPerPixel, &lineLen, &endian);
	if (bitsPerPixel % 8 != 0) {
		std::cerr	<< "Error mlx_get_data_addr(): gives the invalid bits_per_pixel parameter '"
					<< bitsPerPixel << "'" << std::endl;
	}
	bytespp = bitsPerPixel / 8;

	clear(white, BOTH);
	mlx_put_image_to_window(mlx, win, ptrShowImg, 0, 0);
}

void	MlxImage::freePointers(void) {
	if (DEBUG_MODE) { std::cout << "Pointers freed:"; }
	while (!pointers.empty()) {
		if (DEBUG_MODE) { std::cout << " " << pointers.top(); }
		free(pointers.top());
		pointers.pop();
		if (DEBUG_MODE && pointers.empty()) { std::cout << "\n"; }
	}
}


// Non-member functions

int		destroyNotify(int button, void* param) {
	(void)button;
	(void)param;
	if (DEBUG_MODE) { std::cout << "destroyNotify\n"; }
	exit(SUCCESS);
	return 0;
}

int		keyDown(int key, void* param) {
	(void)param;
	if (DEBUG_MODE) { std::cout << "keyDown: " << key << "\n"; }
	if (key == KEY_LEFT_CMD		|| key == KEY_RIGHT_CMD ||
		key == KEY_LEFT_CTRL	|| key == KEY_RIGHT_CTRL ||
		key == KEY_LEFT_ALT		|| key == KEY_RIGHT_ALT ||
		key == KEY_LEFT_SHIFT	|| key == KEY_RIGHT_SHIFT)
	{
		var.img->holdKey = key;
	} else if (key == KEY_ESCAPE) {
		var.img->freePointers();
		var.img->~MlxImage();
		exit(SUCCESS);
	}
	return 0;
}

int		keyUp(int key, void* param) {
	(void)param;
	if (DEBUG_MODE) { std::cout << "keyUp: " << key << "\n"; }
	if (key == KEY_LEFT_CMD		|| key == KEY_RIGHT_CMD ||
		key == KEY_LEFT_CTRL	|| key == KEY_RIGHT_CTRL ||
		key == KEY_LEFT_ALT		|| key == KEY_RIGHT_ALT ||
		key == KEY_LEFT_SHIFT	|| key == KEY_RIGHT_SHIFT)
	{
		var.img->holdKey = 0;
	}
	return 0;
}

int		mouseKeyDown(int button, void* param) {
	(void)param;
	if (DEBUG_MODE) { std::cout << "mouseKeyDown: " << button << "\n"; }
	if (button == MOUSE_LEFT_CLICK || button == MOUSE_RIGHT_CLICK ||
		button == MOUSE_MIDDLE_CLICK)
	{
		var.img->mouseHoldKey = button;
	}
	return 0;
}

int		mouseKeyUp(int button, void* param) {
	(void)param;
	if (DEBUG_MODE) { std::cout << "mouseKeyUp: " << button << "\n"; }
	if (button == MOUSE_LEFT_CLICK || button == MOUSE_RIGHT_CLICK ||
		button == MOUSE_MIDDLE_CLICK)
	{
		var.img->mouseHoldKey = 0;
	}
	return 0;
}

int		mouseMove(int button, void* param) {
	(void)param;
	static int x = 0, y = 0;
	(void)button;
	mlx_mouse_get_pos(var.img->get_win(), &x, &y);
	if (DEBUG_MODE && 
		x >= 0 &&
		y >= 0 &&
		x < var.img->get_width() &&
		y < var.img->get_height()) {
		std::cout << "(" << x << "," << var.img->get_height() - y - 1 << ")\n";
	}
	return 0;
}

