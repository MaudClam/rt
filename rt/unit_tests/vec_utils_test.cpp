#include "debug_utils.hpp"
#include "Timer.hpp"
#include "../srcs/math/vec_utils.hpp"

#define PARAMETR_NAME_LENGTH	20
#define TEST_NAME_LENGTH		32
#define DISPLAYS_OF_PROGRESS	20

// g++ -std=c++2a -O2 vec_utils_test.cpp -o vec_utils_test

template <typename T>
std::string get_type_name() {
	if (std::is_same<T, float>::value) return "float";
	if (std::is_same<T, double>::value) return "double";
	if (std::is_same<T, long double>::value) return "long double";
	return "unknown type";
}

template <typename T>
void output_tests_parametrs(T attempts, T eps, T max) {
	char fill = ' ';
	std::cout << "TESTING PARAMETRS" << std::endl;
	std::cout << std::left << std::setw(PARAMETR_NAME_LENGTH) << std::setfill(fill);
	std::cout << "typename:" << get_type_name<T>() << std::endl;
	std::cout << std::left << std::setw(PARAMETR_NAME_LENGTH) << std::setfill(fill);
	std::cout << "number of attempts:" << attempts << std::endl;
	std::cout << std::left << std::setw(PARAMETR_NAME_LENGTH) << std::setfill(fill);
	std::cout << "permissible error:"  << eps << std::endl;
	std::cout << std::left << std::setw(PARAMETR_NAME_LENGTH) << std::setfill(fill);
	std::cout << "max coordinate:"  << max << std::endl;
}

void output_test_start_header(const std::string& str) {
	static int name_length = TEST_NAME_LENGTH;
	if ( int(str.size()) > name_length )
		name_length = int(str.size());
	std::cout << "▶️ " << std::left << std::setw(name_length) << std::setfill(' ') << str;
	std::cout << " started";
}

template <typename T>
void output_visualization_commands(const vec<T,3>& v_inv, const vec<T,3>& n, const vec<T,3>& v) {
	std::cout << "COMMANDS FOR VISUALIZATION IN GEOGEBRA:\n";
	std::cout << "A = (0,0,0)\n";
	std::cout << "I = (" << v_inv << ")\n";
	std::cout << "N = (" << n << ")\n";
	std::cout << "R = (" << v << ")\n";
	std::cout << "Vector(I + A, A)\n";
	std::cout << "Vector(A, A + N)\n";
	std::cout << "Vector(A, A + R)\n";
}

template <typename T>
void test_spherical_conversion(T attempts, T eps, T max) {
	output_test_start_header("test_spherical_conversion()");
	size_t step = attempts / DISPLAYS_OF_PROGRESS;
	vec<T,3> cartesian, spherical, restored;
	vec<T,2> unit_spherical;
	ScopedTimer t;
	for (size_t i = 0; i < attempts; i++) {
		if (i % 2)
			cartesian.random_direction() *= static_cast<T>(math::random_double()) * max;
		else
			cartesian.random_cosine_direction() *= static_cast<T>(math::random_double()) * max;
		spherical.to_spherical(cartesian);
		restored.from_spherical(spherical);
		if ( !(std::abs(restored.x() - cartesian.x()) < eps) ) {
			std::cerr << " ❌ i = " << with_separator(i) << ", mismatch in x(): ";
			std::cerr << restored.x() << " vs " << cartesian.x() << std::endl;
			assert(false);
		}
		if ( !(std::abs(restored.y() - cartesian.y()) < eps) ) {
			std::cerr << " ❌ i = " << with_separator(i) << ", mismatch in y(): ";
			std::cerr << restored.y() << " vs " << cartesian.y() << std::endl;
			assert(false);
		}
		if ( !(std::abs(restored.z() - cartesian.z()) < eps) ) {
			std::cerr << " ❌ i = " << with_separator(i) << ", mismatch in z(): ";
			std::cerr << restored.z() << " vs " << cartesian.z() << std::endl;
			assert(false);
		}
		unit_spherical = vec<T,3>(cartesian).normalize().make_spherical_from_unit();
		if ( !(std::abs(math::normalize_angle(unit_spherical.phi()) - math::normalize_angle(spherical.phi())) < eps) ) {
			std::cerr << " ❌ i = " << with_separator(i) << ", mismatch in phi(): ";
			std::cerr << unit_spherical.phi() << " vs " << spherical.phi() << std::endl;
			assert(false);
		}
		if ( !(std::abs(math::normalize_angle(unit_spherical.theta()) - math::normalize_angle(spherical.theta())) < eps) ) {
			std::cerr << " ❌ i = " << with_separator(i) << ", mismatch in theta(): ";
			std::cerr << unit_spherical.theta() << " vs " << spherical.theta() << std::endl;
			assert(false);
		}
		if (!(i % step)) std::cout << "." << std::flush;
	}
	std::cout << "✅ passed. ";
}

template <typename T>
void test_reflect_direction(T attempts, T eps, bool visualization = false) {
	output_test_start_header("test_reflect_direction()");
	size_t step = attempts / DISPLAYS_OF_PROGRESS;
	vec<T,3> ray;
	vec<T,3> normal;
	vec<T,3> ray_reflected;
	ScopedTimer t;
	for (size_t i = 0; i < attempts; i++) {
		if (i % 2) {
			ray.random_direction();
			normal.random_cosine_direction();
		} else {
			normal.random_direction();
			ray.random_cosine_direction();
		}
		ray_reflected = ray;
		ray_reflected.reflect_direction(normal);
		// The length should remain ≈1
		if ( !(std::abs(ray_reflected.length() - T(1)) < eps) ) {
			std::cerr << " ❌ i=" << with_separator(i) <<", mismatch ray_reflected.length(): ";
			std::cerr << ray_reflected.length() << " vs " << T(1) << std::endl;
			assert(false);
		}
		// The angle between ray_reflected and normal
		// must be symmetrical to the angle between ray and normal
		T dI = ray * normal;
		T dR = ray_reflected * normal;
		if ( !(std::abs(dI + dR) < eps) ) {
			std::cerr << " ❌ i=" << with_separator(i) <<", mismatch angles between rays and normal: ";
			std::cerr << dI << " vs " << dR << std::endl;
			assert(false);
		}

		if (!(i % step)) std::cout << "." << std::flush;
	}
	std::cout << "✅ passed. ";
	if (visualization)
		output_visualization_commands(ray * T(-1), normal, ray_reflected);
}

template <typename T>
void test_refract_schlick_direction(T attempts, T eps, T eta_from, T eta_to, bool visualization = false) {
	output_test_start_header("test_refract_schlick_direction()");
	size_t step = attempts / DISPLAYS_OF_PROGRESS;
	vec<T,3> ray, normal, ray_initial, ray_refracted;
	ScopedTimer t;
	for (size_t i = 0; i < attempts; ++i) {
		// Random ray and normal direction
		if (i % 2) {
			ray.random_direction();
			normal.random_cosine_direction();
		} else {
			normal.random_direction();
			ray.random_cosine_direction();
		}
		ray.normalize();
		normal.normalize();
		ray_initial = ray;
		T eta = (ray * normal < 0) ? (eta_from / eta_to) : (eta_to / eta_from);
		// We apply refraction
		T reflectance = ray.refract_schlick_direction(normal, eta);
		ray_refracted = ray;

		if (reflectance == T(1)) {
			// Total internal reflection - the vector should not change
			if (!(ray_refracted == ray_initial)) {
				std::cerr << " ❌ i=" << with_separator(i) << ", TIR: ray modified\n";
				assert(false);
			}
		} else {
			// Checking normalization
			if (!(std::abs(ray_refracted.length() - T(1)) < eps)) {
				std::cerr << " ❌ i=" << with_separator(i) << ", refracted ray not normalized: ";
				std::cerr << ray_refracted.length() << "\n";
				assert(false);
			}
			// Snell's Law
			T cos_theta1 = -(ray_initial * normal);
			if (cos_theta1 < T(0)) {
				normal = T(-1) * normal;
				cos_theta1 = -(ray_initial * normal);
			}
			const T cos_theta2 = -(ray_refracted * normal);
			const T sin2_theta1 = T(1) - cos_theta1 * cos_theta1;
			const T sin2_theta2 = T(1) - cos_theta2 * cos_theta2;
			const T lhs = eta_from * eta_from * sin2_theta1;
			const T rhs = eta_to   * eta_to   * sin2_theta2;
			if (!(std::abs(lhs - rhs) < eps)) {
				std::cerr << " ❌ i=" << i << ", Snell's law violation:\n";
				std::cerr << "      η₁²⋅sin²θ₁ = " << lhs << "\n";
				std::cerr << "      η₂²⋅sin²θ₂ = " << rhs << "\n";
				assert(false);
			}
		}
		if (!(i % step)) std::cout << "." << std::flush;
	}
	std::cout << "✅ passed. ";
	if (visualization)
		output_visualization_commands(ray_initial * T(-1), normal, ray_refracted);
}

template <typename T>
void test_rotate_around_axis(T attempts, T eps, T max, bool visualization = false) {
	output_test_start_header("test_rotate_around_axis()");
	size_t step = attempts / DISPLAYS_OF_PROGRESS;
	vec<T,3> ray;
	vec<T,3> axis;
	vec<T,3> rotated;
	ScopedTimer t;
	for (size_t i = 0; i < attempts; i++) {
		T angle = static_cast<T>(math::random_double() * math::constants<double>::two_pi);
		if (i % 2) {
			ray.random_cosine_direction() *= static_cast<T>(math::random_double()) * max;
			axis.random_direction();
		} else {
			axis.random_cosine_direction();
			ray.random_direction() *= static_cast<T>(math::random_double()) * max;
		}
		rotated = ray;
		rotated.rotate_around_axis(axis, angle);
		// Preserve length
		if ( !(std::abs(ray.length() - rotated.length()) < eps) ) {
			std::cerr << " ❌ i = " << with_separator(i) <<", mismatch rotated.length(): ";
			std::cerr << rotated.length() << " vs " << ray.length() << std::endl;
			assert(false);
		}
		// Compare the cos of the angles: (axis • v) / |v| should be the same
		T cos_ray = std::abs(axis * ray) / ray.length();
		T cos_rotated = std::abs(axis * rotated) / rotated.length();
		if ( !(std::abs(cos_ray - cos_rotated) < eps) ) {
			std::cerr << " ❌ i = " << with_separator(i) <<", mismatch cos of the angles: ";
			std::cerr << cos_ray << " vs " << cos_rotated << std::endl;
			assert(false);
		}
		// Rotation by 0 — should not change vector
		rotated = ray;
		rotated.rotate_around_axis(axis, 0);
		for(size_t j = 0; j < 3; j++){
			if ( !(std::abs(ray.length() - rotated.length()) < eps) ) {
				std::cerr << " ❌ i = " << with_separator(i) <<", mismatch rotated component (rotation by 0): ";
				std::cerr << rotated[j] << " vs " << ray[j] << std::endl;
				assert(false);
			}
		}
		// Rotation by 2π — should return same vector
		rotated = ray;
		rotated.rotate_around_axis(axis, math::constants<T>::two_pi);
		for(size_t j = 0; j < 3; j++){
			if ( !(std::abs(ray.length() - rotated.length()) < eps) ) {
				std::cerr << " ❌ i = " << with_separator(i) << ", mismatch rotated component (rotation by 2π): ";
				std::cerr << rotated[j] << " vs " << ray[j] << std::endl;
				assert(false);
			}
		}
		// Compare the cos of the angles: (axis • v) / |v| should be the same
		T cos1 = std::abs(axis * ray) / ray.length();
		T cos2 = std::abs(axis * rotated) / rotated.length();
		assert(std::abs(cos1 - cos2) < eps);
		if (!(i % step)) std::cout << "." << std::flush;
	}
	std::cout << "✅ passed. ";
	if (visualization) {
		rotated = ray;
		rotated.rotate_around_axis(axis, math::constants<T>::pi / 2);
		output_visualization_commands(ray * T(-1), axis, rotated);
	}
}


int main() {
	size_t number_of_attempts	= 1e+9;
	{
		using T = double;
		T permissible_error		= 1e-8;
		T max_coordinate		= 1e+3;
		ScopedTimer t;
		output_tests_parametrs<T>(number_of_attempts, permissible_error, max_coordinate);
		//	Tests
		test_spherical_conversion<T>(number_of_attempts, permissible_error, max_coordinate);
		test_reflect_direction<T>(number_of_attempts, permissible_error);
		test_refract_schlick_direction<T>(number_of_attempts, permissible_error, 1.5, 1. / 1.5);
		test_rotate_around_axis<T>(number_of_attempts, permissible_error, max_coordinate);
	}
	std::cout << std::endl;
	{
		using T = float;
		T permissible_error		= 1e-3;
		T max_coordinate		= 10;
		ScopedTimer t;
		output_tests_parametrs<T>(number_of_attempts, permissible_error, max_coordinate);
		//	Tests
		test_spherical_conversion<T>(number_of_attempts, permissible_error, max_coordinate);
		test_reflect_direction<T>(number_of_attempts, permissible_error);
		test_refract_schlick_direction<T>(number_of_attempts, permissible_error, 1.5, 1. / 1.5);
		test_rotate_around_axis<T>(number_of_attempts, permissible_error, max_coordinate);
	}
	std::cout << std::endl;
	return 0;
}
