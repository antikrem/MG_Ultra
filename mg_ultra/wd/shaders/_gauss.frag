#version 330 core
#define GAUSSIAN_CONSTANT_MAX_LENGTH 20

//Applies a gaussian blur to the entire screen
uniform sampler2D scene;
uniform sampler2D brights;

uniform float gaussianFactors[GAUSSIAN_CONSTANT_MAX_LENGTH];
uniform int gaussianFactorslength;
uniform vec2 offsetDirection;

in vec2 uv;
uniform float resolutionScale;

out vec4 colour;
layout(location = 0) out vec4 bloom;

void main() { 
	vec2 offset = offsetDirection /  (resolutionScale * textureSize(brights, 0));

	vec4 sceneTexel = gaussianFactors[0] * texture(brights, resolutionScale * uv).rgba;

	for (int i = 1; i < gaussianFactorslength; i++) {
		sceneTexel += gaussianFactors[i] * texture(brights, resolutionScale * uv + i * offset).rgba;
		sceneTexel += gaussianFactors[i] * texture(brights, resolutionScale * uv - i * offset).rgba;
	}

    bloom = sceneTexel.rgba;
}