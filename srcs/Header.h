#ifndef HEADER_H
# define HEADER_H

# include <iostream>

# define CREATORS			"mclam, cvignar"
# define DEBUG_PATH			"/Users/uru/Library/Mobile Documents/com~apple~CloudDocs/42_Projects/rt/"
# define CONTROLS_FILE		"scenes/controls"
# define PARSING_LOGFILE	"parsing.log"
# define SUCCESS					0
# define ERROR						-1
# define DEBUG_MODE					true
# define DEBUG_KEYS					false
# define DEBUG_MOUSE				false
# define RESOLUTION_MIN 			100
# define RESOLUTION_MAX 			1920
# define DEFAULT_RESOLUTION 		800,600
# define GAMMA						float(2.0)
# define BACKGRND_VISIBILITY_FACTOR	3
# define DAFAULT_SMOOTHING_FACTOR	1
# define DEFAULT_RECURSION_DEPTH	5
# define DAFAULT_PATHS_PER_RAY		pprs[2]
# define BASE						Vec3f(0,0,0),Vec3f(0,0,1)
# define PRECISION					1e-9
# define EPSILON 					1e-3
# define _INFINITY					1e+2
# define MATTNESS_GLOSSY_RATIO		10
# define MATTNESS_GLOSSY_LIMIT		0.3f

# define MAX_PHOTON_COLLISIONS		1000
# define MAX_PHOTONS_NUMBER			1e+6
# define MAX_ESTIMATE_PHOTONS		1e+2
# define MAX_PHOTONS_GRID_STEP		1.0f
# define TOTAL_PHOTONS_POWER		1.0f
# define PHOTON_SURVIVAL			0.9f

# define PIXELS_PER_THREAD			4000
# define RESTORE_RAYS				1
# define RESET_RAYS					2
# define TAKE_PICTURE				3
# define RAYS_TRACING				4

const int pprs[] = {0,25,50,100,200};	// set of values ​​of the variable 'paths'

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
	SMOOTHING_FACTOR,
	RECURSION_DEPTH,
	PHOTON_MAP,
	PATHS_PER_RAY,
	OTHER,
	AMBIENT_LIGHTING,
	DIRECT_LIGHTING,
	BACKGROUND_LIGHT,
	RAYTRACING,
	PATHTRACING
};

class MlxImage;
struct Scene;

struct Var {
	MlxImage*	img;
	Scene*		scene;
	Var(void) : img(NULL), scene(NULL) {}
	~Var(void) {}
};

#endif /* HEADER_H */
