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


out vec3 lightVolumeWorldPosition;
out vec4 screenSpaceCenter;

//MVP for sprites in 3D
uniform mat4 MVP;

void main() {
	vec3 adjustedLightPosition = vec3(lightPosition.xy, -lightPosition.z);
	
	outLightPosition = adjustedLightPosition;
	outLightColour = lightColour;
	outA = a;
	outB = b;
	outC = c;

	lightVolumeWorldPosition = range * vertexPosition + adjustedLightPosition;
	screenSpaceCenter = MVP * vec4(lightPosition, 1);

	gl_Position 
		= MVP * vec4(lightVolumeWorldPosition, 1);
}