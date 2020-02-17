#version 330 core
//Splits scene into lows and brights
#include "helper.glsl"

//full scene
uniform sampler2D scene;
uniform float bloomThreshold;

in vec2 uv;

layout(location = 0) out vec4 bloom;


void main() { 
	vec4 sceneColour = texture(scene, uv).rgba;
    
	bloom = sceneColour * step(bloomThreshold, dot(sceneColour.rgb, COLOUR_PERCEPTION));
}