#ifndef SCENE_HPP
#define SCENE_HPP

#include "math/vec_utils.hpp"
#include "MethodInvoker.hpp"
#include <iostream>
#include <sstream>

struct Scene;
using str_t = std::string;
using istr_t = std::istringstream;
using parserMap_t = MethodInvoker<std::function<istr_t&(istr_t&)>>;

struct Scene {
	parserMap_t parserMap;
	str_t		header;
	int			ambient;
	Scene(void);
	Scene(const Scene& other) = default;
	~Scene(void) = default;
	istr_t& H(istr_t& is);
	istr_t& A(istr_t& is);
};

#endif
