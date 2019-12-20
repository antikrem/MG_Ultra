#version 330 core
#include "helper.glsl"

//sets up the front depth buffer for the next depth peel
uniform sampler2D spriteColour;
uniform sampler2D worldPosition;
uniform sampler2D normals;
uniform sampler2D lightingSensitivity;
uniform sampler2D frontDepthBuffer;

in vec2 uv;

layout(location = 0) out float frontDepth;

void main() {
	frontDepth = texture(frontDepthBuffer, uv).x;
}