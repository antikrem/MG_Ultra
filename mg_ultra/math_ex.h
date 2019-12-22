//extended mathematical functionality
#ifndef __MATH_EX__
#define __MATH_EX__

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
		T difference = min(abs(target - current), rate);
		return current > target ? current - difference : current + difference;
	}
}

#endif