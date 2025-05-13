#include "debug_utils.hpp"
#include "../srcs/math/math_utils.hpp"


int main() {
	std::cout << "TESTING math_utils.hpp" << std::endl;
	std::cout << "----------------------\n" <<std::endl;

	using namespace math;
	using Cf = constants<float>;
	using Cd = constants<double>;

	
	EVAL( round_to_eps((unsigned char)77)				);
	EVAL( round_to_eps((long long)77)					);
	EVAL_FMT( std::boolalpha, round_to_eps((bool)99)	);
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

	EVAL( clamp(31.0f, 15.0f, 30.0f)	);
	EVAL( clamp(15.0001, 15.0, 30.0)	);
	EVAL( clamp(-15.f, 15.0f, 30.0f)	);
	std::cout << std::endl;

	EVAL( normalize_angle(8.77)			);
	EVAL( normalize_angle(-8.77)		);
	EVAL( normalize_angle(0)			);
	EVAL( normalize_angle(Cd::two_pi)	);
	EVAL( normalize_angle(Cf::pi)		);
	std::cout << std::endl;

	EVAL( to_radians(180.0f)								);
	EVAL( to_radians(1.0)									);
	EVAL( to_degrees(to_radians(1.0))						);
	EVAL( to_degrees(normalize_angle(to_radians(999.0)))	);
	std::cout << std::endl;
	
	EVAL( random_double()					);
	std::cout << std::endl;

	return 0;
}
