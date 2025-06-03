
#include "debug_utils.hpp"
#include "../srcs/math/onb.hpp"
#include "../srcs/math/vec_utils.hpp"

using namespace math;

template <typename T>
void test_onb_basic() {
    onb<T> o1;
    assert((o1.n == vec<T,3>(0, 0, 1)));
    assert((o1.b == vec<T,3>(0, 1, 0)));
    assert((o1.t == vec<T,3>(1, 0, 0)));

    // Test build
    vec<T,3> normal(0, 1, 0);
    o1.build(normal);
    assert(std::abs(dot(o1.n, normal) - 1) < constants<T>::eps);
    assert(std::abs(dot(o1.n, o1.b)) < constants<T>::eps);
    assert(std::abs(dot(o1.n, o1.t)) < constants<T>::eps);
    assert(std::abs(dot(o1.b, o1.t)) < constants<T>::eps);

    // Test round-trip transformation
    vec<T,3> v(1, 2, 3), tmp(v), restored(v);
    vec_to_local(o1, tmp);
    vec_from_local(o1, tmp);
    assert((tmp - restored).length() < constants<T>::eps);
}

template <typename T>
void test_onb_rotation() {
	{
		onb<T> o;
		o.yaw(constants<T>::pi_half);
		assert(std::abs(o.n * vec<T,3>(0, 0, 1) - 1) < constants<T>::eps);  // n should remain z+
		assert(std::abs(o.t * vec<T,3>(0, 1, 0) - 1) < constants<T>::eps); // t rotated around z axis
	}
	{
		onb<T> o;
		o.pitch(constants<T>::pi_half);
		assert(std::abs(o.b * vec<T,3>(0, 1, 0) - 1) < constants<T>::eps); // b unchanged
	}
	{
		onb<T> o;
		o.roll(constants<T>::pi_half);
		// After full rotations, lengths must be preserved
		assert(std::abs(o.n.length() - 1) < constants<T>::eps);
		assert(std::abs(o.t.length() - 1) < constants<T>::eps);
		assert(std::abs(o.b.length() - 1) < constants<T>::eps);
	}
	{
		onb<T> o;
		assert(o.is_orthonormal()); // identity basis
		o.yaw(constants<T>::pi_half);
		assert(o.is_orthonormal());
		o.pitch(constants<T>::pi_half);
		assert(o.is_orthonormal());
		o.roll(constants<T>::pi_half);
		assert(o.is_orthonormal());
	}
}

template <typename T>
void test_onb_orthonormality() {
	onb<T> o;

	assert(o.is_orthonormal());

	o.yaw(constants<T>::pi_half);
	assert(o.is_orthonormal());

	o.pitch(constants<T>::pi_half);
	assert(o.is_orthonormal());

	o.roll(constants<T>::pi_half);
	assert(o.is_orthonormal());
	{
		onb<T> o;
		// Multiple turns
		o.yaw(constants<T>::pi_half / 2);
		o.pitch(constants<T>::pi_half / 3);
		o.roll(constants<T>::pi_half / 4);
		assert(o.is_orthonormal());
	}
	{
		onb<T> o;
		o.t.x() += constants<T>::eps * T(10); // специально искажаем t
		assert(!o.is_orthonormal()); // теперь база должна быть некорректной
	}

}


int main() {
	
	
    test_onb_basic<float>();
    test_onb_basic<double>();
    test_onb_rotation<float>();
    test_onb_rotation<double>();
	test_onb_orthonormality<float>();
	test_onb_orthonormality<double>();
    std::cout << "✅ All onb tests passed.\n";
    return 0;
}
