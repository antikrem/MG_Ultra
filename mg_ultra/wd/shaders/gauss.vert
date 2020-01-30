#version 330 core

//Simple screen space quad
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 inputUV;

out vec2 uv;
out float outResolutionFactor;

void main() {
	uv = inputUV;
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0); 
} 