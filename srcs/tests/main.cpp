//
//  main.cpp
//  tests
//
//  Created by uru on 17/11/2024.
//

#include "../geometry.hpp"

int main(void) {
	Vec3f		v;
	const int	n = 4;
	const float	step = M_PI / n;
	float phi = 0., theta = step;
	
	for (; theta < M_PI; theta += step) {
		phi = 0;
		for (; phi < M_2PI; phi += step) {
			v.sphericalDirection2cartesian(phi, theta);
			float p = phi, t = theta;
			v.cartesian2sphericalDirection(p, t);
			std::cout	<< roundedString(radian2degree(phi),4) << " "
						<< roundedString(radian2degree(theta),4) << "  "
						<< "\t";
			p = radian2degree(p);
			t = radian2degree(t);
			std::cout	<< roundedString(p,4) << " "
						<< roundedString(t,4) << "  "
						<< std::endl;
		}
	}
	// insert code here...
	std::cout << "Hello, World!\n";
	return 0;
}
