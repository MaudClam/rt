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

struct ARGBColor;
class MlxImage;

struct Var {
	MlxImage*	img;
	
	Var(void) : img(NULL) {}
	~Var(void) {}
};

#endif /* HEADER_H */
