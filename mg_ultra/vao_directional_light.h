/*vao for directional lights*/
#ifndef __VAO_DIRECTIONAL_LIGHT__
#define __VAO_DIRECTIONAL_LIGHT__

#include "screen_space_consts.h"

#include "vao_buffer.h"

#include "directional_light.h"

class VAODirectionalLight : public VAOBuffer<VAODirectionalLight, DirectionalData> {
	//quad index, index corresponds to VAO
	GLuint quadID = 0;

public:
	void setUpAttribPointers() {
		//Generate and bind base quad buffer
		glGenBuffers(1, &quadID);
		glBindBuffer(GL_ARRAY_BUFFER, quadID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SCREEN_SPACE_QUAD), SCREEN_SPACE_QUAD, GL_STATIC_DRAW);

		//Generate and bind instance buffer
		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, maxSize * sizeof(DirectionalData), NULL, GL_DYNAMIC_DRAW);

		//set up base quad pointer
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quadID);
		glVertexAttribPointer(
			0, //layout = 0
			3, //vec3
			GL_FLOAT,
			GL_FALSE,
			0, //no need to stride
			(void*)0 //vertexes starting at 0
		);

		//set up colour pointer
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glVertexAttribPointer(
			1, //layout = 1
			3, //vec3
			GL_FLOAT,
			GL_FALSE,
			sizeof(DirectionalData), //stride the length of boxdata
			(void*)(offsetof(DirectionalData, DirectionalData::colour)) //colour part of directional lighting
		);

		//set up direction pointer
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glVertexAttribPointer(
			2, //layout = 2
			3, //vec3
			GL_FLOAT,
			GL_TRUE, //normalise direction
			sizeof(DirectionalData), //stride the length of boxdata
			(void*)(offsetof(DirectionalData, DirectionalData::direction)) //direction part of directional lighting
		);

		//set up striding
		glVertexAttribDivisor(0, 0); //base quad
		glVertexAttribDivisor(1, 1); //colour
		glVertexAttribDivisor(2, 1); //direction
	}

	void render(int count) {
		//draw screen space, count is the number of directional lights
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, count);
	}

	VAODirectionalLight(int maxSize) : VAOBuffer<VAODirectionalLight, DirectionalData>(maxSize) {

	}
};

#endif