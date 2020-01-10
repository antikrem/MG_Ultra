#version 330 core
#include "helper.glsl"

//Combines geometry data and light values into scene for post processing
uniform sampler2D spriteColour;
uniform sampler2D spriteWorldPosition;
uniform sampler2D normals;
uniform sampler2D lightingSensitivity;
uniform sampler2D minimumAmbient;
uniform sampler2D directionalLightScene;

uniform float ambientStrength;
uniform vec3 ambientColor;

in vec2 uv;

layout(location = 0) out vec4 color;

void main() {
	vec4 texelColor = texture(spriteColour, uv).rgba;
	float ls = texture(lightingSensitivity, uv).r;
	vec3 ambient = max(ls * ambientStrength, texture(minimumAmbient, uv).r) * texelColor.xyz * ambientColor;
	vec3 directional = ls * texture(directionalLightScene, uv).rgb;

	//combine individual components
	vec3 sceneColour = ambient + directional + vec3(0);

	color = vec4(sceneColour, texelColor.a);
}