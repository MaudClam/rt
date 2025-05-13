#include "Scene.hpp"
#include "vec.hpp"
#include <iostream>

int main() {
	vec3f v;
	vec3f n = {0,0,1};
	v.random_direction();
	std::cout << v << std::endl;
	std::cout << v.reflect_direction(n) << std::endl;
}
