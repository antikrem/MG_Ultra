#version 330 core

#define MAX_TEXTURES 16

//Contains all currently loaded mgt
uniform sampler2D mgtSamplers[MAX_TEXTURES];

in vec3 worldPosition;
in vec2 uv;
in vec2 wl;
in vec2 texSize;
in float render3D;

layout(location = 0) out vec3 color;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 lightingSensitivity;

void main() {
	vec2 ad_wl = mod(wl, texSize);
	//adjust sampling: flip y coordinate
	vec4 texel = texture( mgtSamplers[0], uv + ad_wl).rgba;
	if (texel.a < 0.2) {
		discard;
	}

	color = texel.xyz;
	position =  worldPosition;
	lightingSensitivity = render3D * vec3(1.0f);
}