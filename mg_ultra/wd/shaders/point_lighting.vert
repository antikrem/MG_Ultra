//
#version 330 core

//in
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 lightPosition;
layout(location = 2) in vec3 lightColour;
layout(location = 3) in float a;
layout(location = 4) in float b;
layout(location = 5) in float c;
layout(location = 6) in float range;

//out
out vec3 outLightPosition;
out vec3 outLightColour;
out float outA;
out float outB;
out float outC;

out vec3 worldPosition;

//MVP for sprites in 3D
uniform mat4 MVP;

void main() {
	outLightPosition = lightPosition;
	outLightColour = lightColour;
	outA = a;
	outB = b;
	outC = c;

	worldPosition = range * vertexPosition + lightPosition;

	gl_Position 
		= MVP * vec4(worldPosition, 1);
}