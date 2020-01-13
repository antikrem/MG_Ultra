/*First shader that projects sprites unto 3d worldspace
*/
#version 330 core

//in
layout(location = 0) in vec3 basePos;
layout(location = 1) in vec3 centerPos;
layout(location = 2) in vec2 size;
layout(location = 3) in float theta;
layout(location = 4) in vec4 textureCoordinate; //xy is center, zw is width/length
layout(location = 5) in float wrapFactor; //will overshoot texturing to allow wrapping, usually 1
layout(location = 6) in float spriteTransparency; //transparency
layout(location = 7) in float lightingSensitivity; //extra factor multiplied with lighting
layout(location = 8) in float ambientMinimum; //minimum value of ambient lighting

//out
out vec3 worldPosition;
out vec2 uv; //top left texture offset within atlas
out vec2 wl; //scaling sample position
out vec2 texSize; //the total size used for wrapping
out vec2 speenSpace; //screen space location
out float transparency;
out float inputLightSensitivity;
out float inputAmbientMinimum;

//MVP for sprites in 3D
uniform mat4 MVP;

void main() {
	float rTheta = radians(theta);

    mat3 rotation = mat3(
		cos(rTheta), -sin(rTheta), 0,
		sin(rTheta), cos(rTheta), 0,
		0, 0, 1
	);

	//size of texture, used for wrapping
	texSize = vec2( textureCoordinate.z, textureCoordinate.w);
	//top left texture offset
	uv = textureCoordinate.xy - 0.5 * textureCoordinate.zw;
	//accessor within the texture, mirroring y
	wl = wrapFactor * vec2((basePos.x + 0.5f) * textureCoordinate.z, (0.5f - basePos.y) * textureCoordinate.w)
		- texSize * ((wrapFactor - 1.0f) / 2.0f);

	worldPosition = rotation * (basePos * vec3(size,1)) + centerPos;

	//Z coordinate has to be flipped
	worldPosition.z = -worldPosition.z;

	transparency = spriteTransparency;

	inputLightSensitivity = lightingSensitivity;
	inputAmbientMinimum = ambientMinimum;

	gl_Position 
		= MVP * vec4(worldPosition, 1);
}