#version 330 core

#include "conversion.glsl"

#define MAX_TEXTURES 16

// Contains all currently loaded mgt
uniform sampler2D mgtSamplers[MAX_TEXTURES];

// Viewport sizeof
uniform float viewport_w;
uniform float viewport_h;

// Forward depth buffer
uniform sampler2D frontDepthBuffer;

in vec3 worldPosition;
in vec2 uv;
in vec2 wl;
in vec2 texSize;
in float transparency;
in float inputLightSensitivity;
in float inputAmbientMinimum;
in float modulationStrengthOut;
in vec3 modulationHSLValues;

layout(location = 0) out vec4 color;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normals;
layout(location = 3) out float lightingSensitivity;
layout(location = 4) out float minimumAmbient;
layout(location = 5) out float nextFrontDepthBuffer;

// Conducts hsl colorisation
vec3 colourise(vec3 texel) {
	vec3 hsl = modulationHSLValues;
	float lum = dot(texel, vec3(0.2126, 0.7152, 0.0722));
    
    hsl.b = 1.0 - 2.0 * hsl.b;
    
    if (hsl.b > 0.0) {
    	lum = lum * (1.0 - hsl.b);
        lum += 1.0 - (1.0 - hsl.b);
    }
    else if (hsl.b < 0.0) {
        lum = lum * (hsl.b + 1.0);
    }
    
    return hsl2rgb(vec3(hsl.rg, lum));
}

void main() {
	// Check if this fragment is infront of the current closest fragment 
	float z = gl_FragCoord.z / gl_FragCoord.w;

	vec2 depthSampleTex = gl_FragCoord.xy / vec2(viewport_w, viewport_h);

	if (z <= texture(frontDepthBuffer, depthSampleTex).x) {
		discard;
	}

	vec2 ad_wl = mod(wl, texSize);
	// Adjust sampling: flip y coordinate

	// Remove fully transparent parts
	vec4 texel = texture(mgtSamplers[0], uv + ad_wl).rgba;

	float trans = transparency * texel.a;

	if (trans < 0.01) {
		discard;
	}

	// Compute and mix modulated color
	vec3 modded = colourise(texel.rgb);
	texel.rgb = mix(texel.rgb, modded, modulationStrengthOut);

	// Colours are pre multiplied by alpha
	color =  trans * vec4(texel.rgb , 1.0);
	position = worldPosition;

	// Normal for sprites always face towards the camera
	normals = vec3(0.0, 0.0, 1.0);


	lightingSensitivity = inputLightSensitivity;
	minimumAmbient = inputAmbientMinimum;

	// Position for front depth
	nextFrontDepthBuffer = z;
}