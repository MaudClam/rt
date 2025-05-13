//
//  main.cpp
//  test
//
//  Created by uru on 20/11/2024.
//

#include <iostream>

int main(int argc, const char * argv[]) {
	// insert code here...
	float f = 0.3, f1 = -0.3;
	std::cout << f << " " << f1 << " Hello, World!\n";

	int i = std::floor(f), i1 = std::floor(f1);
	std::cout << i << " " << i1 << " Hello, World!\n";
	
	i = std::round(f); i1 = std::round(f1);
	std::cout << i << " " << i1 << " Hello, World!\n";
	
	i = f; i1 = f1;
	std::cout << i << " " << i1 << " Hello, World!\n";
	return 0;
}
