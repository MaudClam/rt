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
# include <stack>
# include "Header.h"

struct Var;

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
	KEY_LEFT_CMD		= 259,
	KEY_RIGHT_CMD		= 260,
	KEY_LEFT_CTRL		= 256,
	KEY_RIGHT_CTRL		= 269,
	KEY_LEFT_SHIFT		= 257,
	KEY_RIGHT_SHIFT		= 258,
	KEY_LEFT_ALT		= 261,
	KEY_RIGHT_ALT		= 262,
	KEY_ESCAPE			= 53,
	KEY_SPACE			= 49,
	KEY_ARROW_UP		= 126,
	KEY_ARROW_DOWN		= 125,
	KEY_ARROW_RIGHT		= 124,
	KEY_ARROW_LEFT		= 123,
	KEY_1				= 18,
	KEY_2				= 19,
	KEY_3				= 20,
	KEY_4				= 21,
	KEY_5				= 23,
	KEY_6				= 22,
	KEY_7				= 26,
	KEY_8				= 28,
	KEY_9				= 25,
	KEY_0				= 29,
	KEY_c				= 8,
	MOUSE_LEFT_CLICK	= 1,
	MOUSE_RIGHT_CLICK	= 2,
	MOUSE_MIDDLE_CLICK	= 3,
	MOUSE_SCROLL_UP		= 4,
	MOUSE_SCROLL_DOWN	= 5
};

struct ImageOptions {
	const ARGBColor	black		= ARGBColor(  0,   0,   0);
	const ARGBColor	darkGray	= ARGBColor( 32,  32,  32);
	const ARGBColor	lightGray	= ARGBColor(200, 200, 200);
	const ARGBColor	white		= ARGBColor(255, 255, 255);
	const ARGBColor	red			= ARGBColor(255,   0,   0);
	const ARGBColor	green		= ARGBColor(  0, 255,   0);
	const ARGBColor	blue		= ARGBColor(  0,   0, 255);
	ImageOptions(void) {}
	~ImageOptions(void) {}
};

class MlxImage : public ImageOptions {
private:
	void*				mlx;
	void*				win;
	void*				image;
	char*				data;
	std::stack<void*>	pointers;
	int					width;
	int					height;
	int					bytespp;
	int					lineLen;
	int					endian;
public:
	int					holdKey;
	int					mouseHoldKey;
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
	int		get_lineLen(void) const;
	int		get_endian(void) const;
	char*	get_pixelAddr(char* data, const Vec2i& v) const;
	char*	get_pixelAddr(char* data, int x, int y) const;
	Vec2i	get_XY(char* data, char* addr) const;
	void	fill(char* data, const ARGBColor& color);
	void	init(const std::string& header, int w, int h);
	void	freePointers(void);
	bool	isInWinowXY(const Vec2i& v) const;
	void	rtToMlxXY(Vec2i& v) const;
	void	mlxToRtXY(Vec2i& v) const;
};


// Non-member functions

void	_exit(Var& var, int code);
int		destroyNotify(int button, void* param);
int		keyDown(int key, void* param);
int		keyUp(int key, void* param);
int		mouseKeyDown(int button, void *param);
int		mouseKeyUp(int button, void *param);
int		mouseMove(int button, void *param);
bool	isNumericKey(int key);
bool	isHoldKey(int key);
int 	keyToNumber(int key);
void	changeCamera(Var& var, int key);

#endif /* MLXIMAGE_HPP */
