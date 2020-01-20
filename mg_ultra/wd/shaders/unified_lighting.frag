#version 330 core
#include "helper.glsl"

//Combines geometry data and light values into scene for post processing
uniform sampler2D spriteColour;
uniform sampler2D spriteWorldPosition;
uniform sampler2D normals;
uniform sampler2D lightingSensitivity;
uniform sampler2D minimumAmbient;
uniform sampler2D frontDepthBuffer;
uniform sampler2D lightScene;

uniform float ambientStrength;
uniform vec3 ambientColor;

uniform float fogStrength;
uniform float fogClip;
uniform float fogMax;
uniform vec3 fogColour;

in vec2 uv;

layout(location = 0) out vec4 color;

void main() {
//fragment depth
	float depth = texture(frontDepthBuffer, uv).r;

	//derive texel strength, considering fog
	vec4 texelColor = texture(spriteColour, uv);

	texelColor.rgb = mix(
		fogColour, 
		texelColor.rgb,
		clamp(1.0 / exp((depth - fogClip) * fogStrength), 1 - fogMax,1)
	);
	
	float ls = texture(lightingSensitivity, uv).r;
	vec3 ambient = max(ls * ambientStrength, texture(minimumAmbient, uv).r) * texelColor.xyz * ambientColor;
	vec3 light = ls * texture(lightScene, uv).rgb;

	
	//combine individual components
	vec3 sceneColour = 
		ambient + light + vec3(0);
		

	color = vec4(sceneColour, texelColor.a);
}