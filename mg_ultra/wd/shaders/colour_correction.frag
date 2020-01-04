#version 330 core
//Applies filmic tonmapping 

//full scene
uniform sampler2D scene;
//brights
uniform sampler2D brights;

uniform float exposure;

in vec2 uv;

out vec4 colour;

//some more tonemapping options
vec3 reinhardTonemap(vec3 colour) {
	return colour / (colour + 1.0) * (exposure / exposure);
}

vec3 filmicTonemap(vec3 colour) {
	colour = max(vec3(0.0), colour - 0.004);
	return (colour * (6.2 * colour + 0.5)) / (colour * (6.2 * colour + 1.7) + 0.06);
}

vec3 ACESFilmTonemap(vec3 colour) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((colour * (a * colour + b))/(colour * (c * colour + d) + e), 0.0, 1.0);
}

vec3 exposureMapping(vec3 colour) {
    vec3 mapped = vec3(1.0) - exp(-colour * exposure);
    return pow(mapped, vec3(1.0 / 2.2));
}

void main() {
	vec4 bright = texture(brights, uv);
	vec4 lows = texture(scene, uv);

	//mix the lows and the highs
	vec3 sceneTexel 
		= bright.rgb * bright.a + lows.rgb * (1.0 - bright.a);

    colour = vec4(
		exposureMapping(sceneTexel.rgb), 
		1.0
	);
}