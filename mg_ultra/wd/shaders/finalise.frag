#version 330 core
//Renders to screen
uniform sampler2D scene;

in vec2 uv;

out vec4 colour;

void main() { 
	vec3 sceneTexel = texture(scene, uv).rgb;
    colour = vec4(
		sceneTexel.rgb, 
		1.0
	);
}