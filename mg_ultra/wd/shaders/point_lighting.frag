#version 330 core

#include "helper.glsl"

// Need to use lambertian cutoff until normals are added
#define LAMBERT_CUTOFF -0.0

// Settings
uniform int enableVolumetricScattering;
uniform float lightExtinctionLevel;
uniform float debug;

// Uniform sampler2D 
uniform sampler2D noise;
uniform sampler2D spriteColour;
uniform sampler2D spriteWorldPosition;
uniform sampler2D normals;

// Viewport sizeof
uniform float viewport_w;
uniform float viewport_h;
uniform vec3 cameraPos;

uniform float lightScatteringFactor;
uniform vec3 fogColour;
uniform vec2 fogDrift;

in vec3 vertexOffset;
in vec3 outLightPosition;
in vec3 outLightColour;
in float outA;
in float outB;
in float outC;

in float outRange;
in float outStrength;

in vec3 lightVolumeWorldPosition; 

uniform vec3 cameraPosition;

layout(location = 0) out vec4 color;

//intergral of light strength function relative to z
float calculateZIntergral(float z, float offset) {
	float inside = (2 * outA * length(vec2(z, offset)) + outB) / sqrt(4 * outA * outC - pow(outB,2));

	return (2 * atan(inside)) / sqrt(4 * outA * outC - pow(outB,2));
}

//calculates the offset point, the closest point from light postition
//to the line from frag position to camera position
vec3 calculateOffsetPoint(vec3 worldspacePosition) {
	//return 
	//length(cross((outLightPosition - cameraPosition), (outLightPosition - cameraPosition))) 
	/// length(worldspacePosition - cameraPosition);
	vec3 dir = normalize(worldspacePosition - cameraPosition);
	return worldspacePosition + 
		(dot((outLightPosition - worldspacePosition), dir) / dot(dir, dir)) * dir;
}



float calculateLightScattering(vec3 fragPos) {
	//calculate offset point
	vec3 offsetPoint = calculateOffsetPoint(fragPos);
	
	//calculate offset from closest points
	float offset = length(offsetPoint - outLightPosition);
	
	//compute worldspace depth
	float backDepth = outLightPosition.z - fragPos.z;

	float backScatter = max(
		calculateZIntergral(offset, backDepth) 
			- calculateZIntergral(offset, 0), 
		0
	);
	
	float frontDepth = length(outLightPosition - cameraPosition);

	float frontScatter = max(
		calculateZIntergral(offset, frontDepth) 
			- calculateZIntergral(offset, 0), 
		0
	)
	//Remove front scatter if the frag pos is infront of the light
	* step(fragPos.z - outLightPosition.z, 0);

	return max((backScatter + frontScatter) * lightScatteringFactor * ( pow((outRange - offset) / outRange, 2)) - 0.0045, 0);
}

void main() {
	vec2 uv = gl_FragCoord.xy / vec2(viewport_w, viewport_h);

	vec3 worldspace = texture(spriteWorldPosition, uv).rgb;
	vec4 texel = texture(spriteColour, uv).rgba;
	vec3 lightDirection = worldspace - outLightPosition;

	//perlin noise is availible in square texture, requiring differnt uv
	vec2 perlinUV = (gl_FragCoord.xy / vec2(viewport_h, viewport_h)) - 0.8 * (fogDrift / 1024);
	float perlin = texture(noise, perlinUV).r;

	lightDirection = reflect(lightDirection, vec3(0,0,1));
	
	vec3 temp = outLightColour + vec3(viewport_w) + vec3(viewport_h);

	//compute a angle scaling factor
	float lambertFactor = cos_angle(
		lightDirection, 
		texture(normals, uv).rgb
	);

	//distance based light strength
	float lightStrength = max( 
		outStrength / (outA * pow(length(lightDirection), 2) + outB * length(lightDirection) + outC) - lightExtinctionLevel,
		0
	);

	//compute light scattering
	vec4 volScatter = vec4(0);

	if (enableVolumetricScattering != 0) {
		float lightScatter = calculateLightScattering(worldspace);
		volScatter = vec4(outLightColour * fogColour * lightScatter, lightScatter);
	}

	//output 
	color 
		//direct lighting upon fragment strength
		= lightStrength * vec4(outLightColour * texel.xyz, 1) * step(LAMBERT_CUTOFF, lambertFactor) +
		//add volumetric component
		+ volScatter * perlin
		+ 1000 * vec4(debug);	
}