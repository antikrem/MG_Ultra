#version 330 core

#define MAX_TEXTURES 16

//Contains all currently loaded mgt
uniform sampler2D mgtSamplers[MAX_TEXTURES];

in vec2 uv;

layout(location = 0) out vec3 color;

void main() {
	vec4 texel = texture( mgtSamplers[0],vec2(uv.x, uv.y)).rgba;
	if (texel.a < 0.2) {
		discard;
	}
	color = texel.xyz;
}