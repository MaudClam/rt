#include "debug_utils.hpp"
#include "../srcs/math/math_utils.hpp"

//g++ -std=c++2a math_utils_test.cpp -o math_utils_test && ./math_utils_test

using namespace math;

void test_math_utils_basic() {
	std::cout << "▶️ Running test_math_utils_basic()...\n" << std::endl;

	using Cf = constants<float>;
	using Cd = constants<double>;

	EVAL( round_to_eps((unsigned char)77)				);
	EVAL( round_to_eps((long long)77)					);
	EVAL_FMT( std::boolalpha, round_to_eps((bool)99)	);
	EVAL_FMT( std::boolalpha, round_to_eps((bool)0)	);
	EVAL( round_to_eps(1.123456789, 0.0001)				);
	EVAL( round_to_eps(1.123456789f, 1e-3f)				);
	EVAL_FMT( std::setprecision(40), Cd::eps			);
	EVAL_FMT( std::setprecision(40), Cf::eps			);
	EVAL_FMT( std::setprecision(40), round_to_eps(1.123456789876543212345678987654321 ) );
	EVAL_FMT( std::setprecision(40), round_to_eps(1.123456789876543212345678987654321f) );
	std::cout << std::endl;

	EVAL( -0.0f						);
	EVAL( clean_zero(-0.0f)			);
	EVAL( clean_zero(int(-0.0f))	);
	std::cout << std::endl;

	EVAL( generate_nan<double>()			);
	EVAL( de_nan(generate_nan<double>())	);
	EVAL( generate_nan<float>()				);
	EVAL( de_nan(generate_nan<float>())		);
	std::cout << std::endl;

	EVAL( safe_division(1.0, 0.0)		);
	EVAL( safe_division(1.0f, 0.0f)		);
	EVAL( safe_division(1.0, 0.0, 2.0)	);
	std::cout << std::endl;

	EVAL( clamp(31.0f, 15.0f, 30.0f)	);
	EVAL( clamp(15.0001, 15.0, 30.0)	);
	EVAL( clamp(-15.f, 15.0f, 30.0f)	);
	std::cout << std::endl;
	
	EVAL_FMT( std::boolalpha, almost_equal(1.0, 2.0, 1.1)	);
	EVAL_FMT( std::boolalpha, almost_equal<float>(1, 2, 2)	);
	std::cout << std::endl;
	
	EVAL( random_double()					);
	std::cout << std::endl;
	std::cout << "✅ test_math_utils_basic() passed.\n" << std::endl;

}

template <typename T>
void test_math_utils_angles() {
	std::cout << "▶️ Running test_math_utils_angles<" << get_type_name<T>() << ">()...\n";
	using C = constants<T>;
	
	EVAL_FMT_( "", normalize_angle(0), "0");
	EVAL_FMT_( "", normalize_angle(-0), "0");
	EVAL_FMT_( "", normalize_angle(C::pi_half), "1.5708");
	EVAL_FMT_( "", normalize_angle(-C::pi_half), "4.71239");
	EVAL_FMT_( "", normalize_angle(C::pi), "3.14159");
	EVAL_FMT_( "", normalize_angle(-C::pi), "3.14159");
	EVAL_FMT_( "", normalize_angle(C::two_pi), "0");
	EVAL_FMT_( "", normalize_angle(-C::two_pi), "0");
	EVAL_FMT_( "", normalize_angle(C::pi * T(81)), "3.14159");
	EVAL_FMT_( "", normalize_angle(-C::pi * T(81)), "3.14159");
	EVAL_FMT_( "", normalize_angle(C::pi * T(80)), "0");
	EVAL_FMT_( "", normalize_angle(-C::pi * T(80)), "0");
	std::cout << "-------------" << std::endl;
	EVAL_FMT_( "", wrap_angle(0), "0");
	EVAL_FMT_( "", wrap_angle(-0), "0");
	EVAL_FMT_( "", wrap_angle(C::pi_half * T(801)), "1.5708");
	EVAL_FMT_( "", wrap_angle(-C::pi_half * T(801)), "-1.5708");
	EVAL_FMT_( "", wrap_angle(C::pi), "-3.14159");
	EVAL_FMT_( "", wrap_angle(-C::pi), "-3.14159");
	EVAL_FMT_( "", wrap_angle(C::two_pi), "0");
	EVAL_FMT_( "", wrap_angle(-C::two_pi), "0");
	EVAL_FMT_( "", wrap_angle(C::pi * T(39)), "-3.14159");
	EVAL_FMT_( "", wrap_angle(-C::pi * T(39)), "-3.14159");
	EVAL_FMT_( "", wrap_angle(C::pi * T(800)), "0");
	EVAL_FMT_( "", wrap_angle(-C::pi * T(800)), "0");

	std::cout << "✅ test_math_utils_angles<" << get_type_name<T>() << ">() passed.\n" << std::endl;
}

template <typename T>
void test_math_utils_angles_1() {
	std::cout << "▶️ Running test_math_utils_angles_1<" << get_type_name<T>() << ">()...\n";
	using C = constants<T>;
	
	EVAL_FMT_( std::boolalpha, almost_equal_angles(T(0), C::two_pi), "true");
	EVAL_FMT_( std::boolalpha, almost_equal_angles(T(0), -C::two_pi), "true");
	EVAL_FMT_( std::boolalpha, almost_equal_angles(C::pi, -C::pi), "true");
	EVAL_FMT_( std::boolalpha, almost_equal_angles(C::two_pi, -C::two_pi), "true");
	EVAL_FMT_( std::boolalpha, almost_equal_angles(T(0), C::two_pi * T(10000)), "true");
	EVAL_FMT_( std::boolalpha, almost_equal_angles(T(0), -C::two_pi * T(10000)), "true");
	EVAL_FMT_( std::boolalpha, almost_equal_angles(C::pi, -C::pi * T(10001)), "true");
	EVAL_FMT_( std::boolalpha, almost_equal_angles(C::two_pi, -C::two_pi * T(10000)), "true");
	std::cout << "-------------" << std::endl;
	EVAL_FMT_( std::boolalpha, compare_angles(T(0), T(1)), "true");
	EVAL_FMT_( std::boolalpha, compare_angles(T(-1), T(0)), "true");
	EVAL_FMT_( std::boolalpha, compare_angles(T(0), C::pi - T(0.1)), "true");
	EVAL_FMT_( std::boolalpha, compare_angles(T(-3), T(2)), "false");
	EVAL_FMT_( std::boolalpha, compare_angles(T(2), T(-3)), "true");
	EVAL_FMT_( std::boolalpha, compare_angles(T(1), T(0)), "false");
	EVAL_FMT_( std::boolalpha, compare_angles(T(0), T(-1)), "false");
	EVAL_FMT_( std::boolalpha, compare_angles(C::pi - T(0.1), T(0)), "false");
	EVAL_FMT_( std::boolalpha, compare_angles(T(0), C::pi), "false");
	EVAL_FMT_( std::boolalpha, compare_angles(C::pi, T(0)), "false");
	EVAL_FMT_( std::boolalpha, compare_angles(T(0), -C::pi), "false");
	EVAL_FMT_( std::boolalpha, compare_angles(-C::pi, T(0)), "false");
	EVAL_FMT_( std::boolalpha, compare_angles(T(0), T(2) * C::pi), "false");
	EVAL_FMT_( std::boolalpha, compare_angles(T(0), T(0)), "false");
	EVAL_FMT_( std::boolalpha, compare_angles(T(0), C::pi_half), "true");
	EVAL_FMT_( std::boolalpha, compare_angles(T(0), T(1e-45)), "true");
	EVAL_FMT_( std::boolalpha, compare_angles(T(0), T(1e-46)), "true");
	EVAL_FMT_( std::boolalpha, compare_angles(C::pi, T(-10000 * C::two_pi + C::pi + C::eps)), "true");
	EVAL_FMT_( std::boolalpha, compare_angles(C::pi, T(-1 * C::two_pi + C::pi + C::eps)), "true");
	EVAL_FMT_( std::boolalpha, compare_angles(C::pi, T(-10000 * C::two_pi + C::pi - C::eps)), "false");

	std::cout << "✅ test_math_utils_angles_1<" << get_type_name<T>() << ">() passed.\n" << std::endl;
}


int main() {
	system("clear");
	std::cout << "TESTING math_utils.hpp" << std::endl;
	std::cout << "----------------------\n" <<std::endl;
//	⚠️ These tests are sensitive to the accuracy of Double / Float
//	and show the limits of the correct work Compare_angles ().
	
	test_math_utils_basic();
	test_math_utils_angles<double>();
	test_math_utils_angles<float>();
	test_math_utils_angles_1<double>();
	test_math_utils_angles_1<float>();

	return 0;
}
