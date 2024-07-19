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
# include "MlxImage.hpp"
# include "Ray.hpp"
# include "AScenery.hpp"
# include "Sphere.hpp"
# include "Scene.hpp"
# include "Camera.hpp"

# define SUCCESS		0
# define ERROR			-1
# define EPSILON 		1e-5
# define DEBUG_MODE		true
# define DEBUG_KEYS		false
# define DEBUG_MOUSE	false

class MlxImage;
struct Scene;

struct Var {
	MlxImage*	img;
	Scene*		scene;
	Var(void) : img(NULL) {}
	~Var(void) {}
};

#endif /* HEADER_H */
