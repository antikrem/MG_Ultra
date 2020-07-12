//extended mathematical functionality
#ifndef __MATH_EX__
#define __MATH_EX__

#include <algorithm>
#include <tuple>

#include "constants.h"

using namespace std;

namespace math_ex {
	// Templated clamp
	template <typename T>
	T clamp(const T& value, const T& min, const T& max) {
		return value > max ? max : (value < min ? min : value);
	}

	// Templated tend to
	// Will tend current towards target at rate
	// Will not overshoort
	// Tate must be non-negative
	template<typename T>
	T tend_to(const T& current, const T& rate, const T& target) {
		T difference = min(abs(target - current), rate);
		return current > target ? current - difference : current + difference;
	}

	// Converts to principle range
	float to_principle(float angle);

	// Computes shortest turn to target from current
	float compute_smallest_turn(float target, float current);

	// Solves quadratic equation
	template<typename T>
	tuple<T, T> solve_quadratic(T a, T b, T c) {
		T sqrtDisc = sqrt(SQUARE(b) - ((T)4) * a * c);

		return make_tuple(
			(-b - sqrtDisc) / (2 * a),
			(-b + sqrtDisc) / (2 * a)
		);
	}

	// Solves quadratic given y
	template<typename T>
	tuple<T, T> solve_quadratic(T a, T b, T c, T y) {
		T sqrtDisc = sqrt(SQUARE(b) - ((T)4) * a * (c-y));

		return make_tuple(
			(-b - sqrtDisc) / (2 * a),
			(-b + sqrtDisc) / (2 * a)
		);
	}

	// Extrapolates a length through a perspective shift in z axis
	// Angle would be half fov
	template<typename T>
	T perp_z(T length, T depth, T angle) {
		return length + tan(DEG2RAD(angle)) * depth;
	}

	// Does comparison with given error tolerence
	template<typename T>
	bool comp(T a, T b, T e) {
		return abs(a-b) < e;
	}
}

#endif