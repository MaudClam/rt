#include "debug_utils.hpp"
#include "../srcs/math/vec_utils.hpp"


int main() {
	std::cout << "TESTING vec.hpp" << std::endl;
	std::cout << "---------------\n" <<std::endl;

	vec<float,3> a, b(1,2,3), c = { 0,1,0 }; std::cout << "vec<float,3> a, b(1,2,3), c = { 0,1,0 }: " << a << ", " << b << ", " << c << std::endl;
	EVAL( a = b );
	EVAL( b = c );
	EVAL( c[1] );
	std::cout << std::endl;

	EVAL( a.set(1,2,3).map([](double x) { return -x; }) );
	EVAL( map(a.set(1,2,3), [](double x) { return -x; }) );
	EVAL( a.set(1,2,3).zip_with(b.set(4,5,6), std::plus<>{}) );
	EVAL( zip_with(a.set(1,2,3), b.set(4,5,6), std::plus<>{}) );
	std::cout << std::endl;

	EVAL( c.set(.4f, .5, c[1] / 10) );
	EVAL( c.fill(0. / 0) );
	EVAL( c.de_nan() );
	EVAL( c.clamp(1,2) );
	std::cout << std::endl;

	EVAL( a.set(1,2,3) += b.set(4,5,6) );
	EVAL( a -= b );
	EVAL( c.set(1,2,3) *= 7.1 );
	EVAL( c /= 7.1 );
	EVAL( a.set(1,2,3) + b.set(4,5,6) );
	EVAL( a.set(1,2,3) - b.set(4,5,6) );
	EVAL( c.set(1,2,3) * 7.1f );
	EVAL( 7.1f * c.set(1,2,3) );
	EVAL( c.set(7.1, 14.2, 21.3) / 7.1f );
	std::cout << std::endl;
	
	float eps_f;
	double eps_d;
	EVAL( eps_f = math::constants<float>::eps );
	EVAL( eps_d = math::constants<double>::eps );
	EVAL( c.set(1,1,1) /= eps_f );
	EVAL( c.set(1,1,1) /= eps_d );
	EVAL( (vec<double,3>(1,1,1) /= eps_d) );
	EVAL( c.set(1,1,1) /= 0.0 );
	EVAL( c.set(1,1,1) /= 0.0f );
	EVAL( c.set(0,0,0) /= 0.0 );
	EVAL( c.set(0,0,0) /= 0.0f );
	std::cout << std::endl;

	EVAL( dot(a.set(1,2,3), b.set(4,5,6)) );
	EVAL( a.set(1,2,3) * b.set(4,5,6) );
	EVAL( a.set(1,0,0) ^= b.set(0,1,0) );
	EVAL( cross(a.set(1,0,0), b.set(0,1,0)) );
	EVAL( c.cross(a.set(1,0,0), b.set(0,1,0)) );
	EVAL( a.set(1,0,0) ^ b.set(0,1,0) );
	std::cout << std::endl;

	EVAL( a.set(1,1,1).normalize() );
	EVAL( a.set(eps_f,eps_f,eps_f).normalize() );
	EVAL( a.set(0,eps_f,eps_d).normalize() );
	vec<float,4> e;
	EVAL( e.set(3,4,5,6).length_squared() );
	EVAL( e.set(3,4,5,6).length() );
	EVAL( e.set(0,0,0,0).length() );
	EVAL( e.set(3,4,5,6).min() );
	EVAL( e.set(-3,-4,-5,-6).max() );
	EVAL_FMT(std::boolalpha, a.set(0,0,0).is_null());
	EVAL_FMT(std::boolalpha, a.set(1e-46,0,0).is_null());
	EVAL_FMT(std::boolalpha, a.set(1e-45,0,0).is_null());
	std::cout << std::endl;

	EVAL_FMT( std::boolalpha, (vec<int,3>(0,1,2) == vec<int,3>(0,1,2))	);
	EVAL_FMT( std::boolalpha, (vec<float,2>(0,1) == vec<float,2>(1,0))	);
	EVAL_FMT( std::boolalpha, (vec<int,3>(0,1,2) != vec<int,3>(0,1,2))	);
	EVAL_FMT( std::boolalpha, (vec<float,2>(0,1) != vec<float,2>(1,0))	);
	std::cout << std::endl;

	EVAL( a = b.set(3,4,generate_nan<float>()) );
	EVAL( a.de_nan().normalize() );
	EVAL_FMT( std::boolalpha, (vec<int,3>(0,1,generate_nan<float>()) == vec<int,3>(0,1,generate_nan<float>()))	);
	EVAL_FMT( std::boolalpha, (vec<int,3>(0,1,generate_nan<float>()) != vec<int,3>(0,1,generate_nan<float>()))	);

	std::cout << std::endl;
	return 0;
}
