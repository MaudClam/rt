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

# define SUCCESS			0
# define ERROR				-1
# define DEBUG_MODE			true
# define DEBUG_KEYS			false
# define DEBUG_MOUSE		false
# define RECURSION_DEPTH	25
# define SMOOTHING_FACTOR 	1
# define BASE				Vec3f(0,0,0),Vec3f(0,0,1)

class MlxImage;
struct Scene;

struct Var {
	MlxImage*	img;
	Scene*		scene;
	Var(void) : img(NULL) {}
	~Var(void) {}
};

#endif /* HEADER_H */
