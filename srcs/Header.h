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
# define RECURSION_DEPTH	5
# define SMOOTHING_FACTOR 	1
# define RESOLUTION_MIN 	100
# define RESOLUTION_MAX 	1920
# define DEFAULT_RESOLUTION 800,600
# define BASE				Vec3f(0,0,0),Vec3f(0,0,1)
# define NUM_THREADS		64
# define RESTORE_RAYS		1
# define RESET_RAYS			2
# define TAKE_PICTURE		3
# define RAY_TRACING		4

class MlxImage;
struct Scene;

struct Var {
	MlxImage*	img;
	Scene*		scene;
	Var(void) : img(NULL) {}
	~Var(void) {}
};

#endif /* HEADER_H */
