#version 330 core

#define MAX_TEXTURES 16

//Contains all currently loaded mgt
uniform sampler2D mgtSamplers[MAX_TEXTURES];

//camera clipping distances
uniform float clipNear;
uniform float clipFar;

//forward depth buffer
uniform sampler2D frontDepthBuffer;

in vec3 worldPosition;
in vec2 uv;
in vec2 wl;
in vec2 texSize;

layout(location = 0) out vec3 color;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normals;
layout(location = 3) out vec3 lightingSensitivity;
layout(location = 4) out float nextFrontDepthBuffer;

void main() {
	//check if this fragment is infront of the current closest fragment 
	float z = 
		(2.0 * clipNear * clipFar) /
		(clipFar + clipNear - (2.0 * gl_FragCoord.z - 1.0) * (clipFar - clipNear));

	if (z <= texture(frontDepthBuffer, uv).x) {
		discard;
	}

	vec2 ad_wl = mod(wl, texSize);
	//adjust sampling: flip y coordinate

	//remove fully transparent parts
	vec4 texel = texture(mgtSamplers[0], uv + ad_wl).rgba;
	if (texel.a < 0.2) {
		discard;
	}

	color = texel.xyz;
	position =  worldPosition;

	//normal for sprites always face towards the camera
	normals = vec3(0.0, 0.0, 1.0);
	lightingSensitivity = vec3(1.0f);

	//position for front depth
	nextFrontDepthBuffer = z;
}