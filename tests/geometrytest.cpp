//
//  geometrytest.cpp
//  geometry
//
//  Created by uru on 02/07/2024.
//

#include "../geometry/geometry.hpp"

int main(void) {
	Vec3f v1;
	std::istringstream iss("3.1 ,. .4");
	iss >> v1;
	

	std::cout << v1;
	return 0;
}
