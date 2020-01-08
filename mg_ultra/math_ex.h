//extended mathematical functionality
#ifndef __MATH_EX__
#define __MATH_EX__

#include <algorithm>

namespace math_ex {
	//templated clamp
	template <typename T>
	T clamp(const T& value, const T& min, const T& max) {
		return value > max ? max : (value < min ? min : value);
	}

	//templated tend to
	//will tend current towards target at rate
	//will not overshoort
	//rate must be non-negative
	template<typename T>
	T tend_to(const T& current, const T& rate, const T& target) {
		T difference = std::min(abs(target - current), rate);
		return current > target ? current - difference : current + difference;
	}

	//converts to principle range
	float to_principle(float angle);

	//computes shortest turn to target from current
	float compute_smallest_turn(float target, float current);
}

#endif