#version 330 core
//The first pass will create a frame buffer, this is the colour value
uniform sampler2D firstPassSampler;

in vec2 uv;

out vec4 colour;

void main() { 
    colour = vec4(texture(firstPassSampler, uv).rgb, 1.0);
}