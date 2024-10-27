//
//  Header.h
//  rt
//
//  Created by uru on 07/07/2024.
//

#ifndef HEADER_H
# define HEADER_H

# include "mlx.h"
# include "geometry.hpp"
# include "ARGBColor.hpp"
# include "Ray.hpp"
# include "MlxImage.hpp"
# include "A_Scenery.hpp"
# include "Sphere.hpp"
# include "Scene.hpp"
# include "Light.hpp"
# include "DirectionMatrix.hpp"
# include "camera.hpp"
# include "keys.h"

# define CREATORS			"mclam, cvignar"
# define DEBUG_PATH			"/Users/uru/Library/Mobile Documents/com~apple~CloudDocs/42_Projects/rt/"
# define CONTROLS_FILE		"scenes/controls"
# define PARSING_LOGFILE	"parsing.log"
# define SUCCESS			0
# define ERROR				-1
# define DEBUG_MODE			true
# define DEBUG_KEYS			false
# define DEBUG_MOUSE		false
# define RESOLUTION_MIN 	100
# define RESOLUTION_MAX 	1920
# define DEFAULT_RESOLUTION 800,600
# define BASE				Vec3f(0,0,0),Vec3f(0,0,1)
# define SMOOTHING_FACTOR				1
# define RECURSION_DEPTH				5
# define SOFT_SHADOW_LENGTH_LIMIT		100
# define SOFT_SHADOW_SOFTNESS			1.9
# define PIXELS_PER_THREAD				4000
# define RESTORE_RAYS					1
# define RESET_RAYS						2
# define TAKE_PICTURE					3
# define RAY_TRACING					4

enum Controls {
	OFF,
	ON,
	UNHOLD,
	NEXT				= 101,
	PREVIOUS			= 111,
	INCREASE,
	DECREASE,
	RIGHT,
	LEFT,
	UP,
	DOWN,
	FORWARD,
	BACKWARD,
	YAW_RIGHT,
	YAW_LEFT,
	PITCH_UP,
	PITCH_DOWN,
	ROLL_RIGHT,
	ROLL_LEFT,
	CLOCKWISE,
	COUNTER_CLOCKWISE,
	STEP_FOV			= 1,
	STEP_MOVE			= 1,
	STEP_ROTATION		= 1,
	FLYBY_STEP			= 20,
	FLYBY_RADIUS_MAX	= 100,
	CHANGE_SMOOTHING_FACTOR,
	CHANGE_RECURSION_DEPTH,
	CHANGE_SOFT_SHADOW_LENGTH,
	CHANGE_SOFT_SHADOW_SOFTNESS,
};

class MlxImage;
struct Scene;

struct Var {
	MlxImage*	img;
	Scene*		scene;
	Var(void) : img(NULL), scene(NULL) {}
	~Var(void) {}
};

struct Test {
	std::vector<float>	v;
	Test(void) : v() {}
	~Test(void) {}
	Test(const Test& other) : v(other.v) {}
};

#endif /* HEADER_H */
