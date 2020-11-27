#include "math_ex.h"

float math_ex::to_principle(float angle) {
	angle = fmodf(angle, 360.0f);
	return angle > 180.0f ? angle - 360.0f : angle;
}


float math_ex::compute_smallest_turn(float target, float current) {
	float difference = to_principle(target) - to_principle(current);
	if (-180.0f < difference && difference < 180.0f) {
		return difference;
	}
	else if (180 <= difference) {
		return difference - 360.0f;
	}
	else {
		return difference + 360.0f;
	}
}