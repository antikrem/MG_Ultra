#version 330 core

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

layout(location = 0) out vec4 color;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normals;
layout(location = 3) out float lightingSensitivity;
layout(location = 4) out float minimumAmbient;
layout(location = 5) out float nextFrontDepthBuffer;

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