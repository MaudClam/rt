//
//  colortest.cpp
//  color
//
//  Created by uru on 02/07/2024.
//

#include "ARGBColor.hpp"

int main(void) {
	ARGBColor c(255,254,253);
	ARGBColor c1(0xFFF1F2F3);
	ARGBColor c2(13302345,RGB);
	ARGBColor c3(45,GRAY_SCALE);
	ARGBColor c4; c4 = c1;
	ARGBColor c5;
	std::istringstream is("90");
	is >> c5;
	std::string tail;
	is >> tail;


	std::cout << "c:  " << c << std::endl;
	std::cout << "c1: " << c1 << std::endl;
	std::cout << "c2: " << c2 << std::endl;
	std::cout << "c3: " << c3 << std::endl;
	std::cout << "c4: " << c4 << std::endl;
	std::cout << "c5: " << c5 << std::endl;
	std::cout << "tail: '" << tail << "'" << std::endl;
	return 0;
}
