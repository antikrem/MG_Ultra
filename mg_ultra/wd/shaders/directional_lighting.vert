#version 330 core

//Simple screen space quad
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 colour;
layout (location = 2) in vec3 direction;

out vec2 uv;
out vec3 lightColour;
out vec3 adjustedDirection;


void main() {
	uv = (pos.xy + 1.0) / 2;
	lightColour = colour;
	adjustedDirection = direction;
    gl_Position = vec4(pos.xy, 0.0, 1.0); 
} 