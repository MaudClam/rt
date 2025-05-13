
#include "../Timer.hpp"
#include "../debug_utils.hpp"
#include "../../srcs/math/vec_utils.hpp"
#include "sampling.hpp"
#include <iostream>
#include <iomanip>

// g++ -std=c++2a -O2 sampling_test.cpp -o sampling_test

void test_safe_random_direction() {
	using vec3 = vec<float, 3>;
	constexpr int trials = 100'000'000;
	int hits_clamp = 0;
	int hits_reject = 0;
	ScopedTimer t;
	for (int i = 0; i < trials; ++i) {
		vec3 v1, v2;
		sampling::safe_random_direction<float, true>(v1, hits_clamp);
		sampling::safe_random_direction<float, false>(v2, hits_reject);
	}

	std::cout << "Safe Random Direction Test (" << with_separator(trials) << " samples):\n";
	std::cout << "  Clamp strategy hits:  " << hits_clamp
	          << " (" << std::fixed << std::setprecision(6)
	          << (hits_clamp * 100.0 / trials) << "%)\n";
	std::cout << "  Reject strategy hits: " << hits_reject
	          << " (" << (hits_reject * 100.0 / trials) << "%)\n";
}

void test_safe_random_cosine_direction() {
	using vec3 = vec<float, 3>;
	constexpr int trials = 100'000'000;
	int hits_clamp = 0;
	int hits_reject = 0;
	ScopedTimer t;
	for (int i = 0; i < trials; ++i) {
		vec3 v1, v2;
		sampling::safe_random_cosine_direction<float, true>(v1, hits_clamp);
		sampling::safe_random_cosine_direction<float, false>(v2, hits_reject);
	}

	std::cout << "Safe Random Cosine Direction Test (" << with_separator(trials) << " samples):\n";
	std::cout << "  Clamp strategy hits:  " << with_separator(hits_clamp)
	          << " (" << std::fixed << std::setprecision(6)
	          << (hits_clamp * 100.0 / trials) << "%)\n";
	std::cout << "  Reject strategy hits: " << hits_reject
	          << " (" << (hits_reject * 100.0 / trials) << "%)\n";
}

int main() {
	test_safe_random_direction();
	test_safe_random_cosine_direction();
	return 0;
}
