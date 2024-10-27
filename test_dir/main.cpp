//
//  main.cpp
//  test_dir
//
//  Created by uru on 14/10/2024.
//

#include "DirectionMatrix.hpp"


int main(void) {
	int maxPhi = 360, maxTheta = 180;
	DirMatrix		dM(maxPhi, maxTheta);
	photonRays_t	s;
	Position		pos(Vec3f(0,0,0), Dir(90,90,maxPhi,maxTheta).v);
	
//	std::cout << dM << std::endl;
	std::cout << dM.size() << std::endl;

	dM.randomSampleHemisphereCosineDistribution(s, pos, 20);
	std::cout << s.size() << std::endl;
	for (auto it = s.begin(); it < s.end(); ++it) {
		std::cout << it->dir.roundedOutput() << " ";
		std::cout << Dir(it->dir, maxPhi, maxTheta) << std::endl;
	}
	return 0;
}
