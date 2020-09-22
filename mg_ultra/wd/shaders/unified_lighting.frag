#version 330 core
#include "helper.glsl"

// Combines geometry data and light values into scene for post processing
uniform sampler2D spriteColour;
uniform sampler2D spriteWorldPosition;
uniform sampler2D normals;
uniform sampler2D lightingSensitivity;
uniform sampler2D minimumAmbient;
uniform sampler2D frontDepthBuffer;
uniform sampler2D lightScene;

// Sampling noise
uniform sampler2D noise;

uniform float ambientStrength;
uniform vec3 ambientColor;

uniform float fogStrength;
uniform float fogClip;
uniform float fogMax;
uniform vec3 fogColour;

// Uniforms for modulating colour
uniform float foregroundCutoff;
uniform float backgroundCutoff;
uniform float foregroundStrength;
uniform float backgroundStrength;
uniform vec3 foregroundColour;
uniform vec3 backgroundColour;

in vec2 uv;

layout(location = 0) out vec4 color;

void main() {
	// Fragment depth
	float depth = texture(frontDepthBuffer, uv).r;

	// Compute foreground background mixing ratio
	float fbMix = smoothstep(foregroundCutoff, backgroundCutoff, texture(frontDepthBuffer, uv).x);
	float modStrength = mix(foregroundStrength, backgroundStrength, fbMix);
	vec3 modColour = mix(foregroundColour, backgroundColour, fbMix);

	// Derive texel strength, considering fog
	vec4 texelColor = texture(spriteColour, uv);

	float minAmb =  texture(minimumAmbient, uv).r;

	// Ambient light is affected by fog
	// However, attaching a minimum ambient component
	// Will reduce fog
	texelColor.rgb = mix(
		fogColour * texelColor.a * ambientStrength, 
		texelColor.rgb,
		clamp(1.0 / exp((depth - fogClip) * fogStrength), clamp(1 - fogMax, minAmb, 1), 1)
	);
	
	float ls = texture(lightingSensitivity, uv).r;
	vec3 ambient = max(ls * ambientStrength, minAmb) * texelColor.xyz * ambientColor;
	vec3 light = ls * texture(lightScene, uv).rgb;

	// Combine individual components
	vec3 sceneColour = 
		ambient + light;
	
	// Mix with light modulation
	sceneColour = mix(sceneColour, modColour * sceneColour, modStrength);

	color = vec4(sceneColour, texelColor.a);
}