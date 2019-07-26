/*First shader that projects sprites unto 3d worldspace
*/
#version 330 core

//in
layout(location = 0) in vec3 basePos;
layout(location = 1) in vec3 centerPos;
layout(location = 2) in vec2 size;
layout(location = 3) in float theta;
layout(location = 4) in vec4 textureCoordinate; //xy is center, zw is width/length

//out
out vec2 uv;

uniform mat4 MVP;

void main(){
    mat3 rotation = mat3(
		cos(theta), -sin(theta), 0,
		sin(theta), cos(theta), 0,
		0, 0, 1
	);

	//y coordinate is flipped
	uv = vec2( textureCoordinate.z * basePos.x, 1.0 - textureCoordinate.w * basePos.y) + textureCoordinate.xy;

	gl_Position =  MVP * vec4( (rotation * basePos) * vec3(size,1) + centerPos, 1 );
}