//
//  Scene.hpp
//  rt
//
//  Created by uru on 12/07/2024.
//

#ifndef SCENE_HPP
# define SCENE_HPP

# include "AScenery.hpp"
# include "Sphere.hpp"
# include "Header.h"

struct Scene {
	std::vector<AScenery*>	scenerys;
	std::vector<AScenery*>	objsIdx;
	std::vector<AScenery*>	lightsIdx;
	Scene(void);
	~Scene(void);
	Scene(const Scene& other);
	Scene& operator=(const Scene& other);
	void set_scenery(AScenery* scenery);
	void indexing(void);
};

#endif /* SCENE_HPP */
