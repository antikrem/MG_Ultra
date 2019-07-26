#version 330 core
uniform sampler2D myTextureSampler;

in vec2 uv;

out vec3 color;

void main(){
    vec2(uv.x, 1.0-uv.y);
	vec4 texel = texture( myTextureSampler,vec2(uv.x, uv.y)).rgba;
	if (texel.a < 0.2) {
		discard;
	}
	color = texel.xyz;
}