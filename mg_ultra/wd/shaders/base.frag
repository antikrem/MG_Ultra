#version 330 core

#define MAX_TEXTURES 16

//Contains all currently loaded mgt
uniform sampler2D mgtSamplers[MAX_TEXTURES];

in vec2 uv;
in vec2 wl;
in vec2 texSize;

layout(location = 0) out vec3 color;

void main() {
	vec2 ad_wl = mod(wl, texSize);
	//adjust sampling: flip y coordinate
	vec4 texel = texture( mgtSamplers[0], uv + ad_wl).rgba;
	if (texel.a < 0.2) {
		discard;
	}
	color = texel.xyz;
}