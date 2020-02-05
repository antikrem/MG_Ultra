#version 330 core

#include "helper.glsl"

//in
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 lightPosition;
layout(location = 2) in vec3 lightColour;
layout(location = 3) in float a;
layout(location = 4) in float b;
layout(location = 5) in float c;
layout(location = 6) in float range;
layout(location = 7) in float strength;

//out
noperspective out vec3 vertexOffset;
out vec3 outLightPosition;
out vec3 outLightColour;
out float outA;
out float outB;
out float outC;

flat out float outRange;
flat out float outStrength;

out vec3 lightVolumeWorldPosition;
out vec4 screenSpaceCenter;

//MVP for sprites in 3D
uniform mat4 MVP;
uniform vec3 cameraPosition;

void main() {
	vec3 adjustedLightPosition = flip_z(lightPosition);
	
	outLightPosition = adjustedLightPosition;
	outLightColour = lightColour;
	outA = a;
	outB = b;
	outC = c;

	outRange = range;
	outStrength = strength;

	vertexOffset = range * vertexPosition;
	lightVolumeWorldPosition = range * vertexPosition + adjustedLightPosition;

	gl_Position 
		= MVP * vec4(lightVolumeWorldPosition, 1);
}