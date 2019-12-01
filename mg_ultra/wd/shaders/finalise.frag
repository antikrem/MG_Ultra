#version 330 core
//Renders to screen
uniform sampler2D scene;

in vec2 uv;

out vec4 colour;

void main() { 
    colour = vec4(texture(scene, uv).rgb, 1.0);
}