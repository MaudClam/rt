#ifndef MLXIMAGE_HPP
# define MLXIMAGE_HPP

# include <stack>
# include <unordered_map>
# include "geometry.hpp"
# include "ARGBColor.hpp"
# include "mlx.h"
# include "Scene.hpp"
# include "Header.h"
# include "keys.h"


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


class HoldKeys {
	int	_raw[3];
public:
	HoldKeys(void);
	~HoldKeys(void);
	HoldKeys(const HoldKeys& other);
	HoldKeys& operator=(const HoldKeys& other);
	int  firstEmptyPlace(void);
	int  numEmptyPlaces(void);
	bool isInSet(int key);
	bool isIn(int key);
	bool hold(int key);
	bool unhold(int key);
	bool UNHOLD_(void);
	bool CTRL_(void);
	bool SHIFT_(void);
	bool ALT_(void);
	bool CTRL_SHIFT_(void);
	bool CTRL_ALT_(void);
	bool SHIFT_ALT_(void);
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
	HoldKeys			hold;
	int					mouseHoldKey;
	int					flyby;
	enum ClearWhat { BOTH, DRAW_IMG, SHOW_IMG };

/*
 * Enum class below: A strongly-typed,
 * scoped enumeration that prevents implicit conversion
 * to int and avoids name clashes. Introduced in C++11
 * for safer enum usage.
 * To convert an enum class value to an integer, use:
 *     static_cast<int>(enum_value)
 */
	enum class Command {
		QuitTheProgram,
		CameraSwitching,
		CurrentCameraFOV,
		CurrentCameraMoving,
		CurrentCameraMoving_,
		CurrentCameraRotation,
		FlybyAroundScene,
		CameraOptionSmoothingFactor,
		CameraOptionRecursionDepth,
		CameraOptionPhotonMap,
		CameraOptionPathsPerRay,
		CameraOptionOther,
		CommandCount,
		NotACommand
	};
	const std::unordered_map<Command, std::string> commandDescriptions = {
		{Command::QuitTheProgram,  "Quit The Program"},
		{Command::CameraSwitching,   "Camera Switching"},
		{Command::CurrentCameraFOV,  "Current Camera FOV"},
		{Command::CurrentCameraMoving, "Current Camera Moving"},
		{Command::CurrentCameraMoving_, "Current Camera Moving_"},
		{Command::CurrentCameraRotation, "Current Camera Rotation"},
		{Command::FlybyAroundScene, "Flyby Around Scene"},
		{Command::CameraOptionSmoothingFactor, "Camera Option Smoothing Factor"},
		{Command::CameraOptionRecursionDepth, "Camera Option Recursion Depth"},
		{Command::CameraOptionPhotonMap, "Camera Option Photon Map"},
		{Command::CameraOptionPathsPerRay, "Camera Option Paths Per Ray"},
		{Command::CameraOptionOther, "Camera Option Other"},
		{Command::NotACommand, ""}
	};
	char	lastCommand[100];
	
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
	void	displayCommand(MlxImage::Command command);

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
int		_KEY(bool expression);
int		_NUMERIC(int key);
int		_ARROWS_LEFT_RIGHT(int key);
int		_ARROWS_UP_DOWN(int key);
int		_ARROWS(int key);
int		_MINUS_PLUS(int key);
int		_ARROWS_AND_MINUS_PLUS(int key);
MlxImage::Command	quitTheProgram(bool hold, int val);
MlxImage::Command	camerasSwitching(bool hold, int val);
MlxImage::Command	currentCameraFOV(bool hold, int val);
MlxImage::Command	currentCameraMoving(bool hold, int val);
MlxImage::Command	currentCameraMoving_(bool hold, int val);
MlxImage::Command	currentCameraRotation(bool hold, int val);
MlxImage::Command	flybyAroundTheScene(bool hold, int val);
MlxImage::Command	camerasOptions(int option, bool hold, int val);
//bool	quitTheProgram(bool hold, int val);
//bool	camerasSwitching(bool hold, int val);
//bool	currentCameraFOV(bool hold, int val);
//bool	currentCameraMoving(bool hold, int val);
//bool	currentCameraMoving_(bool hold, int val);
//bool	currentCameraRotation(bool hold, int val);
//bool	flybyAroundTheScene(bool hold, int val);
//bool	camerasOptions(int option, bool hold, int val);
#endif /* MLXIMAGE_HPP */
