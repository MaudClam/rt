//
//  main.cpp
//  test_dir
//
//  Created by uru on 14/10/2024.
//

//#include "geometry.hpp"
//#include "ARGBColor.hpp"
//#include "Power.hpp"
#include "DirMatrix.hpp"

void testDir(void) {
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
}

std::ostream& operator<<(std::ostream& o, const traces_t& traces) {
	for (auto trace = traces.begin(), end = traces.end(); trace != end; ++trace) {
		std::cout << (*trace)->pos << std::endl;
	}
	return o;
}

void testPhotonMap(void) {
	PhotonMap	phMap;
	traces_t	traces;
	
	phMap.set_newTrace(Vec3f(0,2.19,1), Vec3f(0,0,1), Vec3f(1,1,1), CAUSTIC);
	phMap.set_newTrace(Vec3f(0,2.2,1), Vec3f(0,0,1), Vec3f(1,1,1), CAUSTIC);
	phMap.set_newTrace(Vec3f(0,3,1), Vec3f(0,0,1), Vec3f(1,1,1), CAUSTIC);
	phMap.get_traces27(Vec3f(0,2,1), traces, CAUSTIC);
	
	std::cout << phMap.get_size(CAUSTIC) << std::endl;
	std::cout << traces << std::endl;
}


int main(void) {
	testDir();
	testPhotonMap();
	return 0;
}
