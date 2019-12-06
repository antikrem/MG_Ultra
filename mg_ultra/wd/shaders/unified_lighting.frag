#version 330 core
#include "helper.glsl"

//Combines geometry data and light values into scene for post processing
uniform sampler2D spriteColour;
uniform sampler2D spriteWorldPosition;
uniform sampler2D normals;
uniform sampler2D lightingSensitivity;
uniform sampler2D directionalLightScene;

uniform float ambientStrength;
uniform vec3 ambientColor;

in vec2 uv;

layout(location = 0) out vec3 color;

void main() {
	vec3 texelColor = texture(spriteColour, uv).rgb;
	vec3 ls = texture(lightingSensitivity, uv).rgb;
	vec3 ambient = ambientStrength * texelColor * ambientColor;
	vec3 directional = texture(directionalLightScene, uv).rgb;
	color = mix3(ls, ambient, directional, vec3(0));
}