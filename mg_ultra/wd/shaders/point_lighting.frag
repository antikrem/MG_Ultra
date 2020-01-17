#version 330 core

#include "helper.glsl"

//need to use lambertian cutoff until normals are added
#define LAMBERT_CUTOFF -0.1

uniform sampler2D spriteColour;
uniform sampler2D spriteWorldPosition;
uniform sampler2D normals;

//viewport sizeof
uniform float viewport_w;
uniform float viewport_h;

in vec3 outLightPosition;
in vec3 outLightColour;
in float outA;
in float outB;
in float outC;

in vec3 worldPosition; 

layout(location = 0) out vec4 color;

void main() {
	vec2 uv = gl_FragCoord.xy / vec2(viewport_w, viewport_h);

	vec3 worldspace = texture(spriteWorldPosition, uv).rgb;
	vec3 texel = texture(spriteColour, uv).rgb;
	vec3 lightDirection = worldspace - outLightPosition;
	
	vec3 temp = outLightColour + vec3(viewport_w) + vec3(viewport_h);

	//compute a angle scaling factor
	float lambertFactor = cos_angle(
		lightDirection, 
		texture(normals, uv).rgb
	);

	//distance based light strength
	float lightStrength 
		= max( 
		1.0 / (outA * pow(length(lightDirection), 2) + outB * length(lightDirection) + outC) - 0.0045,
		0
	);

	//output 
	if (lambertFactor > LAMBERT_CUTOFF) {
		color =  lightStrength * vec4(outLightColour * texel, 1);
	}
	else {
		color = vec4(0.0);
	}
	
}