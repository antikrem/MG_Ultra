#version 330 core
//Splits scene into lows and brights
#include "helper.glsl"

//full scene
uniform sampler2D scene;
uniform float bloomThreshold;

in vec2 uv;

layout(location = 0) out vec4 bloom;


void main() { 
	vec3 sceneColour = texture(scene, uv).rgb;
    
	if (dot(sceneColour.rgb, COLOUR_PERCEPTION) > bloomThreshold) {
		bloom = vec4(sceneColour, 1.0);
	}
	else {
		bloom = vec4(0);
	}
}