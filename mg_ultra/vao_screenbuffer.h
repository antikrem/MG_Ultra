#ifndef __VAO_SCREENBUFFER__
#define __VAO_SCREENBUFFER__

#include "_graphics_headers.h"

//screen space quad
const float SCREEN_SPACE_QUAD[] = {
	-1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f
};

//uvs for screen space
const float SCREEN_SPACE_UV[] = {
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f
};

/*A vao for rendering an entire screen space
quad, can efficiently handle any screen space quad*/
class VAOScreenBuffer {
private:
	//vao associated with this buffer
	GLuint vaoID;

	//instance buffer, index corresponds to VAO
	GLuint quadVboID;

	//uv buffer
	GLuint uvVboID;

public:
	VAOScreenBuffer() {
		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);
		
		//Generate and bind base quad buffer
		glGenBuffers(1, &quadVboID);
		glBindBuffer(GL_ARRAY_BUFFER, quadVboID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SCREEN_SPACE_QUAD), SCREEN_SPACE_QUAD, GL_STATIC_DRAW);

		//Generate and bind base uv buffer
		glGenBuffers(1, &uvVboID);
		glBindBuffer(GL_ARRAY_BUFFER, uvVboID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SCREEN_SPACE_UV), SCREEN_SPACE_UV, GL_STATIC_DRAW);

		//set up base quad pointer
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quadVboID);
		glVertexAttribPointer(
			0, //layout = 0
			3, //vec3
			GL_FLOAT,
			GL_FALSE,
			0, //no need to stride
			(void*)0 //vertexes starting at 0
		);

		//set up base uv pointer
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvVboID);
		glVertexAttribPointer(
			1, //layout = 1
			2, //vec2
			GL_FLOAT,
			GL_FALSE,
			0, //no need to stride
			(void*)0 //vertexes starting at 0
		);

		//unbind VAO
		glBindVertexArray(0);
	}

	//only need to render
	void processGLSide() {
		glBindVertexArray(vaoID);
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
};
#endif