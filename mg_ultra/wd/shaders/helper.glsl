/*Some helper glsl functions*/

#define COLOUR_PERCEPTION vec3(0.2126, 0.7152, 0.0722)

/*takes vec3 and returns addition of components*/ 
float sum_up(vec3 x) {
	return dot(x, vec3(1.0));
}

/*clamps to non-negative range*/
float clampPositive(float value) {
	return clamp(value, 0, abs(value));
}

/*computes the angle between u and v*/
float cos_angle(vec3 u, vec3 v) {
	return dot(normalize(u), normalize(v));
}

/*computes the angle between u and v*/
float angle(vec3 u, vec3 v) {
	return acos(cos_angle(u, v));
}

/*mixes proportions of a, b and c*/
vec3 mix3(vec3 proportion, vec3 a, vec3 b, vec3 c) {
	return proportion * transpose(mat3(a, b, c));
}

/*mixes proportions of a, b and c, and fills the gap with base*/
vec3 mix3(vec3 proportion, vec3 a, vec3 b, vec3 c, vec3 base) {
	return mix3(proportion, a, b, c) + clampPositive(1 - sum_up(proportion)) * base;
}