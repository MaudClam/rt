#include <iostream>
#include "../srcs/config.hpp"
#include "../srcs/logging/logging_utils.hpp"
#include "../srcs/logging/timing.hpp"

// g++ -std=c++2a -O2 -Wall -Wextra -Werror test_timing.cpp -o test_timing && ./test_timing

namespace rt { Config config; }

int main(int ac, char** av) {
	rt::parse_args(ac, av);
	system("clear");
	using namespace logging;
	TimeCounter	tc;
	int n = 5;
	int N = 5;
	
	for (int j = 0; j < n; ++j) {
		std::cout << std::setw(9) << std::left << "Test(" + std::to_string(N) + ")";
		for (int i = 0; i < N; i++) {
			progress(std::cout, i, N, "⏱️");
			tc.run_ms(1000);
		}
		std::cout << "OK\n";
		N += 2;
	}
	return 0;
}

