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
# include "Sphere.hpp"

#define DEBUG_MODE	true
#define DEBUG_MOUSE	false

#define SUCCESS		0
#define ERROR		-1

class MlxImage;

struct Var {
	MlxImage*	img;
	
	Var(void) : img(NULL) {}
	~Var(void) {}
};

#endif /* HEADER_H */
