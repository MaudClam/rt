//
//  MlxImage.cpp
//  rt
//
//  Created by uru on 06/07/2024.
//

#include "MlxImage.hpp"

extern Var var;

MlxImage::MlxImage() :
mlx(NULL),
win(NULL),
image(NULL),
data(NULL),
scene(NULL),
pointers(),
width(0),
height(0),
bytespp(0),
lineLen(0),
endian(0),
holdKey(UNHOLD),
mouseHoldKey(UNHOLD),
flyby(FLYBY_OFF),
autoFlyby(AUTO_FLYBY_RIGHT)
{}

MlxImage::~MlxImage(void) {
	freePointers();
	if (DEBUG_MODE) { std::cout << "~MlxImage() destructor was done.\n"; }
}

void*	MlxImage::get_mlx(void) const { return mlx; }

void*	MlxImage::get_win(void) const { return win; }

void*	MlxImage::get_image(void) const { return image; }

char*	MlxImage::get_data(void) const { return data; }

Scene*	MlxImage::get_scene(void) const { return scene; }

int		MlxImage::get_width(void) const { return width; }

int		MlxImage::get_height(void) const { return height; }

int		MlxImage::get_bytespp(void) const { return bytespp; }

int		MlxImage::get_lineLen(void) const { return lineLen; }

int		MlxImage::get_endian(void) const { return endian; }

char*	MlxImage::get_pixelAddr(char* data, const Vec2i& v) const {
	if (!data || v.x < 0 || v.y < 0 || v.x >= width || v.y >= height)
		return NULL;
	return data + v.y * width * bytespp + v.x * bytespp;
}

char*	MlxImage::get_pixelAddr(char* data, int x, int y) const {
	Vec2i v(x,y);
	return get_pixelAddr(data, v);
}

Vec2i	MlxImage::get_XY(char* data, char* addr) const {
	Vec2i	v;
	size_t	pos = addr - data;
	
	v.y = (int)(pos / lineLen);
	v.x = (int)( (pos - v.y * lineLen) / bytespp );
	return v;
}

void	MlxImage::set_scene(Scene* scene) { this->scene = scene; }

void	MlxImage::fill(char* data, const ARGBColor& color) {
	if (data) {
		char* dataEnd = data + width * height * bytespp;
		for (; data < dataEnd; data += bytespp) {
			memcpy(data, color.raw, bytespp);
		}
	}
}

void	MlxImage::init(const std::string& header, int w, int h) {
	width = w;
	height = h;

	if ( !(mlx = mlx_init()) ) {
		int _errno = errno;
		std::cerr << "Error mlx_init(): " << strerror(_errno) << std::endl;
		_exit(*var.img, *var.scene, _errno) ;
	}
	pointers.push(mlx);

	if ( !(win = mlx_new_window(mlx, width, height, (char*)header.c_str())) ) {
		int _errno = errno;
		std::cerr << "Error mlx_new_window(): " << strerror(_errno) << std::endl;
		_exit(*var.img, *var.scene, _errno) ;
	}
	pointers.push(win);

	if ( !(image = mlx_new_image(mlx, width, height)) ) {
		int _errno = errno;
		std::cerr << "Error mlx_new_image(): " << strerror(_errno) << std::endl;
		_exit(*var.img, *var.scene, _errno);
	}
	pointers.push(image);

	int bitsPerPixel = 0;
	data = mlx_get_data_addr(image, &bitsPerPixel, &lineLen, &endian);
	if (DEBUG_MODE && (bitsPerPixel % 8 != 0) ) {
		std::cerr	<< "Error mlx_get_data_addr(): gives the invalid bits_per_pixel parameter '"
					<< bitsPerPixel << "'" << std::endl;
	}
	bytespp = bitsPerPixel / 8;

	mlx_hook(win, ON_DESTROY, 0, destroyNotify, NULL);
	mlx_hook(win, ON_KEYDOWN, 0, keyDown, NULL);
	mlx_hook(win, ON_KEYUP, 0, keyUp, NULL);
	mlx_hook(win, ON_MOUSEDOWN, 0, mouseKeyDown, NULL);
	mlx_hook(win, ON_MOUSEUP, 0, mouseKeyUp, NULL);
	mlx_hook(win, ON_MOUSEMOVE, 0, mouseMove, NULL);
}

void	MlxImage::freePointers(void) {
	if (DEBUG_MODE) { std::cout << "Pointers freed:";
		if (pointers.empty()) { std::cout << " not used.\n"; }
	}
	while (!pointers.empty()) {
		if (DEBUG_MODE) { std::cout << " " << pointers.top(); }
		free(pointers.top());
		pointers.pop();
		if (DEBUG_MODE && pointers.empty()) { std::cout << "\n"; }
	}
}

bool	MlxImage::isInWinowXY(const Vec2i& v) const {
	return v.x >= 0 && v.y >= 0 && v.x < width && v.y < height;
}

void	MlxImage::rtToMlxXY(Vec2i& v) const {
	v.toMonitor(width, height);
}

void	MlxImage::mlxToRtXY(Vec2i& v) const {
	v.toRt(width, height);
}


// Non-member functions

void	_exit(MlxImage& img, Scene& scene, int code) {
	img.~MlxImage();
	scene.~Scene();
	exit(code);
}

int		destroyNotify(int button, void* param) {
	(void)button;
	(void)param;
	_exit(*var.img, *var.scene, SUCCESS) ;
	return 0;
}

int		keyDown(int key, void* param) {
	(void)param;
	if (isHoldKey(key)) var.img->holdKey = key;
	if (var.img->holdKey == KEY_LEFT_ALT || var.img->holdKey == KEY_RIGHT_ALT) {
		if (isNumericKey(key)) {
			var.scene->selectCurrentCamera( numericKeyToNumber(key) );
		} else {
			switch (key) {
				case KEY_ARROW_RIGHT: var.scene->selectCurrentCamera(NEXT); break;
				case KEY_ARROW_LEFT:  var.scene->selectCurrentCamera(PREVIOUS);  break;
				case KEY_ARROW_UP:    var.scene->moveCurrentCamera(MOVE_FORWARD); break;
				case KEY_ARROW_DOWN:  var.scene->moveCurrentCamera(MOVE_BACKWARD); break;
				case KEY_PLUS:    	  var.scene->changeCurrentCameraFOV(INCREASE_FOV); break;
				case KEY_MINUS:       var.scene->changeCurrentCameraFOV(DECREASE_FOV); break;
				default: break;
			}
		}
	} else if (var.img->holdKey == KEY_LEFT_SHIFT || var.img->holdKey == KEY_RIGHT_SHIFT) {
		switch (key) {
			case KEY_ARROW_RIGHT: var.scene->rotateCurrentCamera(YAW_RIGHT); break;
			case KEY_ARROW_LEFT:  var.scene->rotateCurrentCamera(YAW_LEFT);  break;
			case KEY_ARROW_UP:    var.scene->rotateCurrentCamera(PITCH_UP); break;
			case KEY_ARROW_DOWN:  var.scene->rotateCurrentCamera(PITCH_DOWN); break;
			case KEY_PLUS:    	  var.scene->rotateCurrentCamera(ROLL_RIGHT); break;
			case KEY_MINUS:       var.scene->rotateCurrentCamera(ROLL_LEFT); break;
			default: break;
		}
	} else {
		switch (key) {
			case KEY_ESCAPE:      _exit(*var.img, *var.scene, SUCCESS);  break;
			case KEY_ARROW_RIGHT: var.scene->moveCurrentCamera(MOVE_RIGHT); break;
			case KEY_ARROW_LEFT:  var.scene->moveCurrentCamera(MOVE_LEFT); break;
			case KEY_ARROW_UP:    var.scene->moveCurrentCamera(MOVE_UP); break;
			case KEY_ARROW_DOWN:  var.scene->moveCurrentCamera(MOVE_DOWN); break;
			default: break;
		}
	}
	if (DEBUG_KEYS) { std::cout << "keyDown: " << key << " holdKey: " << var.img->holdKey << "\n"; }
	return 0;
}

int		keyUp(int key, void* param) {
	(void)param;
	if (var.img->holdKey == key) {
		var.img->holdKey = UNHOLD;
	}
	if (DEBUG_KEYS) { std::cout << "keyUp: " << key << " holdKey: " << var.img->holdKey  << "\n"; }
	return 0;
}

int		mouseKeyDown(int button, void* param) {
	(void)param;
	if (DEBUG_MOUSE) { std::cout << "mouseKeyDown: " << button << "\n"; }
	if (button == MOUSE_LEFT_CLICK || button == MOUSE_RIGHT_CLICK ||
		button == MOUSE_MIDDLE_CLICK)
	{
		var.img->mouseHoldKey = button;
	}
	return 0;
}

int		mouseKeyUp(int button, void* param) {
	(void)param;
	if (DEBUG_MOUSE) { std::cout << "mouseKeyUp: " << button << "\n"; }
	if (button == MOUSE_LEFT_CLICK || button == MOUSE_RIGHT_CLICK ||
		button == MOUSE_MIDDLE_CLICK)
	{
		var.img->mouseHoldKey = 0;
	}
	return 0;
}

int		mouseMove(int button, void* param) {
	(void)param;
	(void)button;
	Vec2i	v;
	mlx_mouse_get_pos(var.img->get_win(), &v.x, &v.y);
	if (DEBUG_MOUSE && var.img->isInWinowXY(v)) {
//		char* pixelAddr = var.img->get_pixelAddr(var.img->get_dataDraw(), v);
//		Vec2i v1 = var.img->get_XY(var.img->get_dataDraw(), pixelAddr);
//		var.img->mlxToRtXY(v1);
//		var.img->rtToMlxXY(v1);
		v.toRt(var.img->get_width(), var.img->get_height());
		std::cout << "(" << v << ")\n";
	}
	return 0;
}

//int		flyby(void) {
//	if (var.img->flyby == FLYBY_ON) {
//		var.scene->moveCurrentCamera(MOVE_RIGHT);
//	}
//	return 0;
//}

bool	isNumericKey(int key) {
	if (key == KEY_1 || key == KEY_2 || key == KEY_3 || key == KEY_4 ||
		key == KEY_5 || key == KEY_6 || key == KEY_7 || key == KEY_8 ||
		key == KEY_9 || key == KEY_0) {
		return true;
	}
	return false;
}

bool	isHoldKey(int key) {
	if (key == KEY_LEFT_CMD		|| key == KEY_RIGHT_CMD ||
		key == KEY_LEFT_CTRL	|| key == KEY_RIGHT_CTRL ||
		key == KEY_LEFT_ALT		|| key == KEY_RIGHT_ALT ||
		key == KEY_LEFT_SHIFT	|| key == KEY_RIGHT_SHIFT) {
		return true;
	}
	return false;
}

int		numericKeyToNumber(int key) {
	switch (key) {
		case KEY_1: { return 1; }
		case KEY_2: { return 2; }
		case KEY_3: { return 3; }
		case KEY_4: { return 4; }
		case KEY_5: { return 5; }
		case KEY_6: { return 6; }
		case KEY_7: { return 7; }
		case KEY_8: { return 8; }
		case KEY_9: { return 9; }
		case KEY_0: { return 0; }
	}
	return (ERROR);
}

