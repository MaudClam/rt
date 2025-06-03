
#include "debug_utils.hpp"
#include "../srcs/math/angle.hpp"
#include "../srcs/math/math_utils.hpp"

//g++ -std=c++2a -O2 -Wall -Wextra -Werror angle_test.cpp -o angle_test && ./angle_test

using namespace math;

template <typename T>
void angle_basic() {
	output_test_start_header(__func__, false);
	using C = constants<T>;

	angle<T> t2;
//	EVAL_FMT_("", t0.set(C::pi/4).output_degrees(), "45º" );
//	EVAL_FMT_("", t1.set(-t0).output_degrees(), "-45º" );

	EVAL_FMT_("", C::pi, "3.14159" );
	EVAL_FMT_("", -C::pi, "-3.14159" );
	EVAL_FMT_("", wrap_angle(C::pi), "-3.14159" );
	EVAL_FMT_("", wrap_angle(-C::pi), "-3.14159" );
	EVAL_FMT_("", wrap_angle(C::two_pi), "0" );
	EVAL_FMT_("", wrap_angle(-C::two_pi), "0" );

	EVAL_FMT_("", t2.set(C::pi), "-3.14159" );
	EVAL_FMT_("", t2.set(-C::pi), "-3.14159" );
	EVAL_FMT_("", t2.set(C::two_pi), "0" );
	EVAL_FMT_("", t2.set(-C::two_pi), "0" );

//	EVAL_FMT_("", t2.set(-C::pi).output_degrees(), "-180º" );
//	EVAL_FMT_("", t0 = t2, "-3.14159" );
	
//	EVAL_FMT_("", t0 = C::pi/4, "0.785398" );
//	EVAL_FMT_("", t0 = -C::pi/4, "-0.785398" );
}

template <typename T>
void angle_range_exit(size_t attempts, T epsilon) {
	using C = constants<T>;
	output_test_start_header(__func__);
	size_t step = attempts / DISPLAYS_OF_PROGRESS;
	angle<T> a;
	size_t count = 0;
	for (size_t i = 0; i < attempts; i++) {
		T sign = i % 2 ? T(-1) : T(1);
		T r = random_around_num<T>(sign * C::pi, epsilon);
		T d = to_degrees(r);
		a.set(r);
		r = a.radians();
		d = a.degrees();
		if (is_range_exit<T>(r, -C::pi, C::pi) || is_range_exit(d, T(-180), T(180)))
			count++;
		if (!(i % step)) std::cout << "." << std::flush;
	}
	if (count)
		std::cerr  << " ❌ " << with_separator(count) << " range exits occurred." << std::endl;
	else
		std::cout << "✅ passed. \n";
}

template <typename T>
void errors_accumulation(T r, int max_cycles = 1e+6) {
	std::string header(__func__);
	header = header + " (" + angle(r).output_degrees() + ")";
	output_test_start_header(header, true, -1);
	int cycles = count_circular_stability<T>(r, [](T r) {
		return angle<T>().set(r).radians();
	}, max_cycles);
	for (int i = 0; i <= DISPLAYS_OF_PROGRESS; ++i) std::cout << ".";
	if (cycles == 0)
		std::cout << "❌ stable cycles: " << cycles << "." << std::endl;
	else if (cycles < max_cycles)
		std::cout << "✅ stable cycles: " << cycles << "." << std::endl;
	else
		std::cout << "✅ reached max_cycles: " << with_separator(max_cycles) << "." << std::endl;
}

template <typename T>
void angle_arithmetic() {
	output_test_start_header(__func__, false);
	using C = constants<T>;
	angle<T> t1, t2;
	
	EVAL_FMT_("", (t1.set(C::pi) + t2.set(C::pi_half)).output_degrees(), "-90º" );
	EVAL_FMT_("", (t1.set(C::pi) + C::pi_half).output_degrees(), "-90º" );
	EVAL_FMT_("", (C::pi_half + t1.set(C::pi)).output_degrees(), "-90º" );
	EVAL_FMT_("", (t1.set(C::pi) - t2.set(C::pi_half)).output_degrees(), "90º" );
	EVAL_FMT_("", (t1.set(C::pi) - C::pi_half).output_degrees(), "90º" );
	EVAL_FMT_("", (C::pi_half - t1.set(C::pi)).output_degrees(), "90º" );
	EVAL_FMT_("", (T(2) * t1.set(C::pi_half)).output_degrees(), "-180º" );
}

template <typename T>
void angle_comparison() {
	output_test_start_header(__func__, false);
	using C = constants<T>;
	angle<T> t1, t2;

//	EVAL_FMT_( std::boolalpha, C::pi == C::pi, "true" );
//	EVAL_FMT_( std::boolalpha, t1.set(C::pi) == C::pi, "true" );
//	EVAL_FMT_( std::boolalpha, C::pi == t1.set(C::pi), "true" );
//	EVAL_FMT_( std::boolalpha, t1.set(C::pi) == t1.set(C::pi), "true" );
//	EVAL_FMT_( std::boolalpha, C::pi == -C::pi, "false" );
//	EVAL_FMT_( std::boolalpha, t1.set(C::pi) == -C::pi, "true" );
//	EVAL_FMT_( std::boolalpha, C::pi == t1.set(-C::pi), "true" );
//	EVAL_FMT_( std::boolalpha, t1.set(C::pi) == t1.set(-C::pi), "true" );
//	EVAL_FMT_( std::boolalpha, t1.set(3.1) == t2.set(3.1) + C::two_pi, "true" );
	std::cout << "----------------" <<std::endl;
	EVAL_FMT_( std::boolalpha, t1.set(C::pi) == t2.set(-C::pi), "true" );
	EVAL_FMT_( std::boolalpha, t1.set(C::pi) != t2.set(-C::pi), "false" );
	EVAL_FMT_( std::boolalpha, t1.set(C::pi) < t2.set(-C::pi), "false" );
	EVAL_FMT_( std::boolalpha, t1.set(C::pi) > t2.set(-C::pi), "false" );
	EVAL_FMT_( std::boolalpha, t1.set(C::pi) <= t2.set(-C::pi), "true" );
	EVAL_FMT_( std::boolalpha, t1.set(C::pi) >= t2.set(-C::pi), "true" );
	std::cout << "----------------" <<std::endl;
	EVAL_FMT_( std::boolalpha, C::pi == t2.set(-C::pi), "true" );
	EVAL_FMT_( std::boolalpha, C::pi != t2.set(-C::pi), "false" );
	EVAL_FMT_( std::boolalpha, C::pi < t2.set(-C::pi), "false" );
	EVAL_FMT_( std::boolalpha, C::pi > t2.set(-C::pi), "false" );
	EVAL_FMT_( std::boolalpha, C::pi <= t2.set(-C::pi), "true" );
	EVAL_FMT_( std::boolalpha, C::pi >= t2.set(-C::pi), "true" );

	
	
	
	
//	EVAL_FMT_( std::boolalpha, C::pi == -C::pi, "false" );
//	EVAL_FMT_( std::boolalpha, t1.set(C::pi) == t2.set(C::pi), "true" );
//	EVAL_FMT_( std::boolalpha, t1.set(C::pi) == t2.set(-C::pi), "true" );
//	EVAL_FMT_( std::boolalpha, t1.set(C::pi) == C::pi, "true" );
//	EVAL_FMT_( std::boolalpha, t1.set(C::pi) == -C::pi, "true" );
//	EVAL_FMT_( std::boolalpha, t2.set(C::pi) != -C::pi, "false" );
//	EVAL_FMT_( std::boolalpha, t1.set(C::pi) == t2.set(C::pi), "true" );
//	EVAL_FMT_( std::boolalpha, t1.set(-C::pi) == t2.set(C::pi), "true" );
//	EVAL_FMT_( std::boolalpha, t1.set(3.1) == t2.set(3.1) + C::two_pi, "true" );

//    angle<T> t1(C::pi);
//    angle<T> t2(-C::pi);
//    assert(t1 == t2);
//    assert(!(t1 < t2));
//    assert(!(t1 > t2));
//    assert(!(t1 != t2));
//
//    angle<T> t3(T(0.5));
//    angle<T> t4(T(1.0));
//    assert(t3 < t4);
//    assert(t4 > t3);
}

template <typename T>
void angle_wrap_and_range() {
	using C = constants<T>;
    angle<T> t1(T(5) * C::pi);
    assert(almost_equal_angles<T>(t1.angle(), wrap_angle<T>(T(5) * C::pi)));

    angle<T> t2(T(-10) * C::pi);
    assert(almost_equal_angles<T>(t2.angle(), wrap_angle<T>(T(-10) * C::pi)));
}

int main() {
//	using Cd = constants<double>;
	using Cf = constants<float>;

	system("clear");
	std::cout << "TESTING angle.hpp" << std::endl;
	std::cout << "----------------\n" <<std::endl;

//    std::cout << "❇️ Running angle<double> tests...\n";
//    angle_basic<double>();
//	errors_accumulation<double>(-Cd::pi, 1e+4);
//	angle_range_exit<double>(1e+7, Cd::eps * double(1e+3));
//    angle_arithmetic<double>();
//    angle_comparison<double>();
//    angle_wrap_and_range<double>();
//    std::cout << "✅ angle<double> passed.\n\n";

	std::cout << "❇️ Running angle<float> tests...\n";
//	angle_basic<float>();
	iterate<float>(to_radians(-1.0f), to_radians(0.2f), to_radians(1.1f), [](double r, int) {
		errors_accumulation(r);
	}, 1e+6);
	iterate<float>(-Cf::pi, Cf::pi/6.0f, Cf::pi - 0.1f, [](float r, int param) {
		errors_accumulation(r, param);
	}, 1e+6);

//	errors_accumulation<float>(0, 1e+4);
//	angle_range_exit<float>(1e+7, Cf::eps * float(1e+3));
//    angle_arithmetic<float>();
//    angle_comparison<float>();
//    angle_wrap_and_range<float>();
	std::cout << "✅ angle<float> passed.\n\n";

    return 0;
}
