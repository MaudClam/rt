//
//  MlxImage.hpp
//  rt
//
//  Created by uru on 06/07/2024.
//

#ifndef MLXIMAGE_HPP
# define MLXIMAGE_HPP
# include <iostream>
# include <cstdlib>
# include <thread>
# include <chrono>
# include <stack>
# include "Header.h"

struct Var;
struct Scene;

enum {
	UNHOLD				= -1,
	ON_KEYDOWN			= 2,
	ON_KEYUP			= 3,
	ON_MOUSEDOWN		= 4,
	ON_MOUSEUP			= 5,
	ON_MOUSEMOVE		= 6,
	ON_EXPOSE			= 12,
	ON_DESTROY			= 17,
	BUTTON_CLOSE_WINDOW	= 0,
	KEY_LEFT_CMD		= 259, // find
	KEY_RIGHT_CMD		= 260, // find
	KEY_LEFT_CTRL		= 65507,
	KEY_RIGHT_CTRL		= 269, // find
	KEY_LEFT_SHIFT		= 65505,
	KEY_RIGHT_SHIFT		= 258, // find
	KEY_LEFT_ALT		= 65513,
	KEY_RIGHT_ALT		= 262, // find
	KEY_ESCAPE			= 65307,
	KEY_SPACE			= 32,
	KEY_ARROW_UP		= 65362,
	KEY_ARROW_DOWN		= 65364,
	KEY_ARROW_RIGHT		= 65363,
	KEY_ARROW_LEFT		= 65361,
	KEY_1				= 49,
	KEY_2				= 50,
	KEY_3				= 51,
	KEY_4				= 52,
	KEY_5				= 53,
	KEY_6				= 54,
	KEY_7				= 55,
	KEY_8				= 56,
	KEY_9				= 57,
	KEY_0				= 48,
	KEY_MINUS			= 45,
	KEY_PLUS			= 61,
	KEY_F				= 102,
	MOUSE_LEFT_CLICK	= 1, // find
	MOUSE_RIGHT_CLICK	= 2, // find
	MOUSE_MIDDLE_CLICK	= 3, // find
	MOUSE_SCROLL_UP		= 4, // find
	MOUSE_SCROLL_DOWN	= 5 // find
};

enum Controls {
	NEXT				= -4,
	PREVIOUS			= -3,
	INCREASE_FOV		= -2,
	DECREASE_FOV		= -1,
	STEP_FOV			= 1,
	MOVE_RIGHT,
	MOVE_LEFT,
	MOVE_UP,
	MOVE_DOWN,
	MOVE_FORWARD,
	MOVE_BACKWARD,
	STEP_MOVE			= 1,
	YAW_RIGHT,
	YAW_LEFT,
	PITCH_UP,
	PITCH_DOWN,
	ROLL_RIGHT,
	ROLL_LEFT,
	STEP_ROTATION		= 1,
	FLYBY_OFF,
	FLYBY_ON,
	FLYBY_STEP			= 20,
	FLYBY_RADIUS_MAX	= 100,
	FLYBY_CLOCKWISE,
	FLYBY_COUNTER_CLOCKWISE
};

struct ImageOptions {
	const ARGBColor	black		= ARGBColor(  0,   0,   0);
	const ARGBColor	darkGray	= ARGBColor( 32,  32,  32);
	const ARGBColor	lightGray	= ARGBColor(200, 200, 200);
	const ARGBColor	white		= ARGBColor(255, 255, 255);
	const ARGBColor	red			= ARGBColor(255,   0,   0);
	const ARGBColor	green		= ARGBColor(  0, 255,   0);
	const ARGBColor	blue		= ARGBColor(  0,   0, 255);
	const ARGBColor	cyan		= ARGBColor(  0, 255, 255);
	const ARGBColor	magenta		= ARGBColor(  255, 0, 255);
	const ARGBColor	yellow		= ARGBColor(  255, 255, 0);
	ImageOptions(void) {}
	~ImageOptions(void) {}
};

class MlxImage : public ImageOptions {
private:
	void*				mlx;
	void*				win;
	void*				image;
	char*				data;
	Scene*				scene;
	std::stack<void*>	pointers;
	int					width;
	int					height;
	int					bytespp;
	int					lineLen;
	int					endian;
public:
	int					holdKey;
	int					mouseHoldKey;
	int					flyby;
	enum ClearWhat { BOTH, DRAW_IMG, SHOW_IMG };
	MlxImage(void);
	~MlxImage();
	void*	get_mlx(void) const;
	void*	get_win(void) const;
	void*	get_image(void) const;
	char*	get_data(void) const;
	int		get_width(void) const;
	int		get_height(void) const;
	int		get_bytespp(void) const;
	int		get_endian(void) const;
	char*	get_pixelAddr(char* data, const Vec2i& v) const;
	char*	get_pixelAddr(char* data, int x, int y) const;
	Vec2i	get_XY(char* data, char* addr) const;
	void	set_scene(Scene* scene);
	void	fill(char* data, const ARGBColor& color);
	void	init(const std::string& header, const Vec2i& resolution);
	void	freePointers(void);
	bool	isInWinowXY(const Vec2i& v) const;
	void	rtToMlxXY(Vec2i& v) const;
	void	mlxToRtXY(Vec2i& v) const;
};


// Non-member functions

void	_exit(MlxImage& img, Scene& scene, int code);
int		destroyNotify(int button, void* param);
int		keyDown(int key, void* param);
int		keyUp(int key, void* param);
int		mouseKeyDown(int button, void *param);
int		mouseKeyUp(int button, void *param);
int		mouseMove(int button, void *param);
int		flyby(void);
bool	isNumericKey(int key);
bool	isHoldKey(int key);
int 	numericKeyToNumber(int key);
void	changeCamera(Var& var, int key);

#endif /* MLXIMAGE_HPP */
