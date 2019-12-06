#version 330 core
//Renders to screen
uniform sampler2D scene;
uniform sampler2D uiScene;

in vec2 uv;

out vec4 colour;

void main() { 
	vec4 uiTexel = texture(uiScene, uv).rgba;
	vec3 sceneTexel = texture(scene, uv).rgb;
    colour = vec4(
		(1.0 - uiTexel.a) * (sceneTexel.rgb) + uiTexel.a * uiTexel.rgb, 
		1.0
	);
}