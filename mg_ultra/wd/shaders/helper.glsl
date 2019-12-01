/*Some helper glsl functions*/

/*takes vec3 and returns addition of components*/ 
float sum_up(vec3 x) {
	return dot(x, vec3(1.0));
}

/*clamps to non-negative range*/
float clampPositive(float value) {
	return clamp(value, 0, abs(value));
}

/*mixes proportions of a, b and c*/
vec3 mix3(vec3 proportion, vec3 a, vec3 b, vec3 c) {
	return proportion * transpose(mat3(a, b, c));
}

/*mixes proportions of a, b and c, and fills the gap with base*/
vec3 mix3(vec3 proportion, vec3 a, vec3 b, vec3 c, vec3 base) {
	return  mix3(proportion, a, b, c) + clampPositive(1 - sum_up(proportion)) * base;
}