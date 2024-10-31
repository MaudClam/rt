//
//  Header.h
//  rt
//
//  Created by uru on 07/07/2024.
//

#ifndef HEADER_H
# define HEADER_H

# include <iostream>

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
# define PRECISION			1e-9
# define EPSILON 			1e-3
# define _INFINITY			100
# define BASE				Vec3f(0,0,0),Vec3f(0,0,1)
# define PHOTON_DIRECTIONS_MATRIX	360,180
# define TOTAL_PHOTONS_NUMBER		1000000
# define PHOTON_MAP_GRID_STEP		0.15
# define SMOOTHING_FACTOR			1
# define RECURSION_DEPTH			5
# define SOFT_SHADOW_LENGTH_LIMIT	100
# define SOFT_SHADOW_SOFTNESS		1.9
# define PIXELS_PER_THREAD			4000
# define RESTORE_RAYS				1
# define RESET_RAYS					2
# define TAKE_PICTURE				3
# define RAY_TRACING				4


//typedef	std::vector<A_Scenery*>	a_scenerys_t;
//typedef	a_scenerys_t::iterator	a_scenerys_it;
//typedef std::vector<Ray>		photonRays_t;
//typedef std::set<Dir>			dirSet_t;
//typedef	std::vector<std::string> stringSet_t;
//typedef	std::vector<float>		floatSet_t;
//typedef	std::vector<Camera>		cameras_t;


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

#endif /* HEADER_H */
