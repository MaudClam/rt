#include "MlxImage.hpp"

extern Var var;

// class HoldKeys

HoldKeys::HoldKeys(void): _raw{UNHOLD,UNHOLD,UNHOLD} {}

HoldKeys::~HoldKeys(void) {}

HoldKeys::HoldKeys(const HoldKeys& other) {
	for (int i = 0; i < 3; i++)
		_raw[i] = other._raw[i];
}

HoldKeys& HoldKeys::operator=(const HoldKeys& other) {
	if (this != &other) {
		for (int i = 0; i < 3; i++)
			_raw[i] = other._raw[i];
	}
	return *this;
}

int  HoldKeys::firstEmptyPlace(void) {
	int i = 0;
	while (i < 3 && _raw[i] != UNHOLD)
		i++;
	if (i < 3)
		return i;
	return ERROR;
}

int  HoldKeys::numEmptyPlaces(void) {
	int num = 0;
	for (int i = 0; i < 3; i++) {
		if (_raw[i] == UNHOLD)
			num++;
	}
	return num;
}

bool HoldKeys::isInSet(int key) {
	switch (key) {
		case KEY_LEFT_CTRL: return true;
		case KEY_RIGHT_CTRL: return true;
		case KEY_LEFT_ALT: return true;
		case KEY_RIGHT_ALT: return true;
		case KEY_LEFT_SHIFT: return true;
		case KEY_RIGHT_SHIFT: return true;
		default: break;
	}
	return false;
}

bool HoldKeys::isIn(int key) {
	int i = 0;
	while (i < 3 && _raw[i] != key)
		i++;
	if (i == 3)
		return false;
	return true;
}

bool HoldKeys::hold(int key) {
	if (isInSet(key)) {
		int i = firstEmptyPlace();
		if (i != ERROR) {
			_raw[i] = key;
			return true;
		}
	}
	return false;
}

bool HoldKeys::unhold(int key) {
	int i = 0;
	while (i < 3) {
		if (_raw[i] == key) {
			_raw[i] = UNHOLD;
			break;
		}
		i++;
	}
	if (i == 3)
		return false;
	return true;
}

bool HoldKeys::UNHOLD_(void) {
	return numEmptyPlaces() == 3;
}

bool HoldKeys::CTRL_(void) {
	return numEmptyPlaces() == 2 && (isIn(KEY_LEFT_CTRL) ^ isIn(KEY_RIGHT_CTRL));
}

bool HoldKeys::SHIFT_(void) {
	return numEmptyPlaces() == 2 && (isIn(KEY_LEFT_SHIFT) ^ isIn(KEY_RIGHT_SHIFT));
}

bool HoldKeys::ALT_(void) {
	return numEmptyPlaces() == 2 && (isIn(KEY_LEFT_ALT) ^ isIn(KEY_RIGHT_ALT));
}

bool HoldKeys::CTRL_SHIFT_(void) {
	return numEmptyPlaces() == 1 && (isIn(KEY_LEFT_CTRL) ^ isIn(KEY_RIGHT_CTRL)) &&
	(isIn(KEY_LEFT_SHIFT) ^ isIn(KEY_RIGHT_SHIFT));
}

bool HoldKeys::CTRL_ALT_(void) {
	return numEmptyPlaces() == 1 && (isIn(KEY_LEFT_CTRL) ^ isIn(KEY_RIGHT_CTRL)) &&
	(isIn(KEY_LEFT_ALT) ^ isIn(KEY_RIGHT_ALT));
}

bool HoldKeys::SHIFT_ALT_(void) {
	return numEmptyPlaces() == 1 && (isIn(KEY_LEFT_SHIFT) ^ isIn(KEY_RIGHT_SHIFT)) &&
	(isIn(KEY_LEFT_ALT) ^ isIn(KEY_RIGHT_ALT));
}


// class MlxImage

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
hold(),
mouseHoldKey(UNHOLD),
flyby(OFF)
{}

MlxImage::~MlxImage(void) {
	freePointers();
}

void*	MlxImage::get_mlx(void) const { return mlx; }

void*	MlxImage::get_win(void) const { return win; }

void*	MlxImage::get_image(void) const { return image; }

char*	MlxImage::get_data(void) const { return data; }

int		MlxImage::get_width(void) const { return width; }

int		MlxImage::get_height(void) const { return height; }

int		MlxImage::get_bytespp(void) const { return bytespp; }

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

void	MlxImage::init(const std::string& header, const Vec2i& resolution) {
	width = resolution.x;
	height = resolution.y;

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
	while (!pointers.empty()) {
		free(pointers.top());
		pointers.pop();
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
	HoldKeys& hold(var.img->hold);
	if ( DEBUG_KEYS ) { std::cout << "keyDown: " << key << "\n"; }
	if ( hold.hold(key) ) { return SUCCESS; }
	
	if ( quitTheProgram(       hold.UNHOLD_(), _KEY(KEY_ESCAPE == key))     ) { return SUCCESS; }
	if ( camerasSwitching(     hold.ALT_(),    _ARROWS_LEFT_RIGHT(key))     ) { return SUCCESS; }
	if ( camerasSwitching(     hold.ALT_(),    _NUMERIC(key))               ) { return SUCCESS; }
	if ( currentCameraFOV(     hold.ALT_(),    _MINUS_PLUS(key))            ) { return SUCCESS; }
	if ( currentCameraMoving(  hold.UNHOLD_(), _ARROWS(key))                ) { return SUCCESS; }
	if ( currentCameraMoving_( hold.ALT_(),    _ARROWS_UP_DOWN(key))        ) { return SUCCESS; }
	if ( currentCameraRotation(hold.SHIFT_(),  _ARROWS_AND_MINUS_PLUS(key)) ) { return SUCCESS; }
	if ( flybyAroundTheScene(  hold.UNHOLD_(), _KEY(KEY_SPACE == key))      ) { return SUCCESS; }
	
	if ( camerasOptions(SMOOTHING_FACTOR,	hold.UNHOLD_(),		_NUMERIC(key))	) { return SUCCESS; }
	if ( camerasOptions(RECURSION_DEPTH,	hold.SHIFT_(),		_NUMERIC(key))	) { return SUCCESS; }
	if ( camerasOptions(PHOTON_MAP,			hold.CTRL_(),		key)			) { return SUCCESS; }
	if ( camerasOptions(PATHS_PER_RAY,		hold.CTRL_(),		_NUMERIC(key))	) { return SUCCESS; }
	if ( camerasOptions(OTHER,				hold.UNHOLD_(),		key)			) { return SUCCESS; }
	return SUCCESS;
}

int		keyUp(int key, void* param) {
	(void)param;
	HoldKeys& hold(var.img->hold);
	if ( DEBUG_KEYS ) { std::cout << "keyUp: " << key << "\n"; }
	if ( hold.unhold(key) ) return SUCCESS;
	// Code for other keys
	return SUCCESS;
}

int		mouseKeyDown(int button, void* param) {
	(void)param;
	if (DEBUG_MOUSE) { std::cout << "mouseKeyDown: " << button << "\n"; }
	if (button == MOUSE_LEFT_CLICK || button == MOUSE_RIGHT_CLICK ||
		button == MOUSE_MIDDLE_CLICK)
	{
		var.img->mouseHoldKey = button;
	}
	return SUCCESS;
}

int		mouseKeyUp(int button, void* param) {
	(void)param;
	if (DEBUG_MOUSE) { std::cout << "mouseKeyUp: " << button << "\n"; }
	if (button == MOUSE_LEFT_CLICK || button == MOUSE_RIGHT_CLICK ||
		button == MOUSE_MIDDLE_CLICK)
	{
		var.img->mouseHoldKey = 0;
	}
	return SUCCESS;
}

int		mouseMove(int button, void* param) {
	(void)param; (void)button;
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
	return SUCCESS;
}

int		flyby(void) {
	if (var.img->flyby != OFF) {
		var.scene->flybyCamera();
	}
	return SUCCESS;
}

int		_KEY(bool expression) {
	if (expression)
		return SUCCESS;
	else
		return ERROR;
}

int		_NUMERIC(int key) {
	switch (key) {
		case KEY_1: key = 1; break;
		case KEY_2: key = 2; break;
		case KEY_3: key = 3; break;
		case KEY_4: key = 4; break;
		case KEY_5: key = 5; break;
		case KEY_6: key = 6; break;
		case KEY_7: key = 7; break;
		case KEY_8: key = 8; break;
		case KEY_9: key = 9; break;
		case KEY_0: key = 0; break;
		default: return ERROR;
	}
	return key;
}

int		_ARROWS_LEFT_RIGHT(int key) {
	switch (key) {
		case KEY_ARROW_LEFT: break;
		case KEY_ARROW_RIGHT: break;
		default: return ERROR;
	}
	return key;
}

int		_ARROWS_UP_DOWN(int key) {
	switch (key) {
		case KEY_ARROW_UP: break;
		case KEY_ARROW_DOWN: break;
		default: return ERROR;
	}
	return key;
}

int		_ARROWS(int key) {
	if (_ARROWS_LEFT_RIGHT(key) != ERROR ^ _ARROWS_UP_DOWN(key) != ERROR) {
		return key;
	}
	return ERROR;
}

int		_MINUS_PLUS(int key) {
	switch (key) {
		case KEY_MINUS: break;
		case KEY_PLUS: break;
		default: return ERROR;
	}
	return key;
}

int		_ARROWS_AND_MINUS_PLUS(int key) {
	if (_ARROWS(key) != ERROR ^ _MINUS_PLUS(key) != ERROR) {
		return key;
	}
	return ERROR;
}

bool	quitTheProgram(bool hold, int val) {
	if (hold && val != ERROR) {
		_exit(*var.img, *var.scene, SUCCESS);
		return true;
	}
	return false;
}

bool	camerasSwitching(bool hold, int val) {
	if (hold && val != ERROR) {
		switch (val) {
			case KEY_ARROW_RIGHT:	var.scene->selectCamera(NEXT); return true;
			case KEY_ARROW_LEFT:	var.scene->selectCamera(PREVIOUS);  return true;
			default:				var.scene->selectCamera(val);  return true;
		}
	}
	return false;
}

bool	currentCameraFOV(bool hold, int val) {
	if (hold && val != ERROR) {
		switch (val) {
			case KEY_PLUS:	var.scene->changeCameraFOV(INCREASE); return true;
			case KEY_MINUS:	var.scene->changeCameraFOV(DECREASE); return true;
		}
	}
	return false;
}

bool	currentCameraMoving(bool hold, int val) {
	if (hold && val != ERROR) {
		switch (val) {
			case KEY_ARROW_RIGHT: var.scene->moveCamera(RIGHT); return true;
			case KEY_ARROW_LEFT:  var.scene->moveCamera(LEFT); return true;
			case KEY_ARROW_UP:    var.scene->moveCamera(UP); return true;
			case KEY_ARROW_DOWN:  var.scene->moveCamera(DOWN); return true;
		}
	}
	return false;
}

bool	currentCameraMoving_(bool hold, int val) {
	if (hold && val != ERROR) {
		switch (val) {
			case KEY_ARROW_UP:   var.scene->moveCamera(FORWARD); return true;
			case KEY_ARROW_DOWN: var.scene->moveCamera(BACKWARD); return true;
		}
	}
	return false;
}

bool	currentCameraRotation(bool hold, int val) {
	if (hold && val != ERROR) {
		switch (val) {
			case KEY_ARROW_RIGHT: var.scene->rotateCamera(YAW_RIGHT); return true;
			case KEY_ARROW_LEFT:  var.scene->rotateCamera(YAW_LEFT);  return true;
			case KEY_ARROW_UP:    var.scene->rotateCamera(PITCH_UP); return true;
			case KEY_ARROW_DOWN:  var.scene->rotateCamera(PITCH_DOWN); return true;
			case KEY_PLUS:    	  var.scene->rotateCamera(ROLL_RIGHT); return true;
			case KEY_MINUS:       var.scene->rotateCamera(ROLL_LEFT); return true;
		}
	}
	return false;
}

bool	flybyAroundTheScene(bool hold,  int val) {
	if (hold && val != ERROR) {
		Camera&	cam(var.scene->cameras[var.scene->get_currentCamera()]);
		int&	flyby(var.img->flyby);
		switch (flyby) {
			case OFF:				cam.calculateFlybyRadius(); flyby = COUNTER_CLOCKWISE; break;
			case COUNTER_CLOCKWISE:	flyby = CLOCKWISE; break;
			case CLOCKWISE:			flyby = OFF; break;
		}
		return true;
	}
	return false;
}

bool	camerasOptions(int option, bool hold, int val) {
	if (hold && val != ERROR) {
		if (option == PHOTON_MAP) {
			switch (val) {
				case KEY_N:	val = NO; break;
				case KEY_C: val = CAUSTIC; break;
				case KEY_G:	val = GLOBAL; break;
				case KEY_V:	val = VOLUME; break;
				default:	return false;
			}
		} else if (option == OTHER) {
			switch (val) {
				case KEY_A:	val = AMBIENT_LIGHTING; break;
				case KEY_D:	val = DIRECT_LIGHTING; break;
				case KEY_B:	val = BACKGROUND_LIGHT; break;
				case KEY_S:	val = SHADOW_RAYS; break;
				case KEY_R:	val = RAYTRACING; break;
				case KEY_P:	val = PATHTRACING; break;
				default:	return false;
			}
		}
		var.scene->changeCamerasOptions(val, option);
		return true;
	}
	return false;
}
