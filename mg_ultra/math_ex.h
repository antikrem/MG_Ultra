//extended mathematical functionality
#ifndef __MATH_EX__
#define __MATH_EX__

namespace math_ex {

	//templated clamp
	template <typename T>
	T clamp(T value, T min, T max) {
		return value > max ? max : (value < min ? min : value);
	}
}

#endif