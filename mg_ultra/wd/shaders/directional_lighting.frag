#version 330 core
#include "helper.glsl"

//calculates light from directional lights
uniform sampler2D spriteColour;
uniform sampler2D spriteWorldPosition;
uniform sampler2D normals;

in vec2 uv;
in vec3 lightColour;
in vec3 adjustedDirection;

layout(location = 0) out vec3 color;

void main() {
	vec3 normal = texture(normals, uv).rgb;
	vec3 texelColor = texture(spriteColour, uv).rgb;

    color = texelColor * lightColour * max(cos_angle(normal, adjustedDirection), 0); 
} 