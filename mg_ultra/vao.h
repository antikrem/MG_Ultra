/*This program uses multiple vao buffers:
There are n vao buffers, which is written to by one thread, graphicsThread, and drawn by another: glThread
Pipeline is:
--Graphics Thread--
graphics thread requests location of last used VAO
graphics thread accesses VAO and requests a mapping pointer
graphics thread waits until mapping pointer is made valid
graphics thread copies all its draw data to VAO and requests draw
graphics thread moves on to next unused buffer
--gl Thread--
checks if a vao request exists
-If it exists, bind, map, unbind

*/

#ifndef __VAO__
#define __VAO__

#include <iostream>

#include "cus_struct2.h"
#include "texture.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Maximum number of boxes able to be rendered
#define BOX_COUNT 500

//base quad that is updated for each instance
const float BASEQUAD[12] = {
	-0.5f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.5f, 0.5f, 0.0f
};

//Vaos can be in in one of multiple states
enum VaoState {
	VS_requestMap,
	VS_validMap,
	VS_requestUnmap,
	VS_readyDraw,
};

//Each vao has a vbo, ibo
//Bind this VAO in render thread
//Render push thread must write to this
class Vao {
	//buffer index
	int bufferIndex;

	//when the VAO is unmapped and set ready to draw, unmapIndex is set to a value larger than all other unmapIndexes
	atomic<long long int> unmapIndex = 0; //todo make sync

	//specifies the lastest unmap
	atomic<long long int> latestUnmapIndex;

	atomic<VaoState> vaoState = VS_readyDraw;

	//dont allow the Vao to be written to when writting
	mutex lock;

	//ID of this Vao
	GLuint vaoID;

	//ID of buffer that stores the base quad
	//This buffer will always be the same
	GLuint quadBufferID;

	//ID of the vbo that holds instanced data
	//This bufffer will be updated with boxdata
	GLuint instanceBufferID;

	//pointer to mapped memory, valid when state is VS_validMap
	BoxData* instanceMap = nullptr;

	//number of boxes to draw, a box is one triangle strip with 4 vertices
	int boxCount = 2;

	//Array of bottom left position (x,y,z), then size (w,h)
	BoxData instanceData[2] = {
		{-0.5f, -0.5f, 0.0f, 1.0f, 1.0f},
		{0.5f, 0.5f, 0.0f, 1.0f, 1.0f}
	};

public:
	VaoState getState() {
		return vaoState.load();
	}

	//Does the gl handling of Vao memory management
	void glHandle() {
		unmapIndex.store(latestUnmapIndex.load());
		if (vaoState == VS_requestMap) {
			glBindVertexArray(vaoID);
			glBindBuffer(GL_ARRAY_BUFFER, instanceBufferID);
			instanceMap = (BoxData*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			glBindVertexArray(0);
			vaoState = VS_validMap;
		}
		else if (vaoState == VS_requestUnmap) {
			glBindVertexArray(vaoID);
			glBindBuffer(GL_ARRAY_BUFFER, instanceBufferID);
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glBindVertexArray(0);
			vaoState = VS_readyDraw;
		}
	}

	//Requests that an unused Vao gets a proper memory buffer, 
	//then blocks thread until this request is met
	//Only is VaoState is VS_unused, otherwised
	BoxData* requestMap() {
		if (vaoState == VS_readyDraw) {
			vaoState = VS_requestMap;
			while (vaoState != VS_validMap) { //todo ez cv
				pass;
			}
			return instanceMap;
		}
		return nullptr;
	}

	//todo error check the next two functions for freeze case

	//asyncronously requests that the vao is set to the VS_validMap state
	//only valid when state is VS_readyDraw
	void requestMapAsync() {
		if (vaoState == VS_readyDraw) {
			vaoState.store(VS_requestMap);
		}
		
	}

	//to be used after requestMapAsync
	//Returns memory space if ready, else blocks
	BoxData* returnMapAsync() {
		if (vaoState == VS_readyDraw) {
			//todo error case, do something
			return nullptr;
		}
		else if (vaoState == VS_requestUnmap) {
			//todo error case, do something
			return nullptr;
		}
		else if (vaoState == VS_validMap) {
			return instanceMap;
		}
		else if (vaoState == VS_requestMap) {
			while (vaoState != VS_validMap) { //todo ez cv
				pass;
			}
			return instanceMap;
		}
		return nullptr;
	}

	//Requests that an unused Vao gets a proper memory buffer, 
	//then blocks thread until this request is met
	//Only is VaoState is VS_unused, otherwised
	void requestUnmap(int boxCount, long long int unmapIndex) {
		if (vaoState == VS_validMap) {
			this->boxCount = boxCount;
			vaoState = VS_requestUnmap;
			while (vaoState != VS_readyDraw) {
				pass;
			}
			this->latestUnmapIndex.store(unmapIndex);
		}
	}

	void drawVao() {
		//Bind this VAO
		glBindVertexArray(vaoID);

		//draw
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, boxCount);

		glBindVertexArray(0);
	}

	Vao(unsigned long long int unmapIndex) {
		this->unmapIndex.store(unmapIndex);

		//Generate and bind VertexArrayObject
		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);

		//Generate and bind base quad buffer
		glGenBuffers(1, &quadBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, quadBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(BASEQUAD), BASEQUAD, GL_STATIC_DRAW);

		//Generate and bind instance buffer
		glGenBuffers(1, &instanceBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, instanceBufferID);
		glBufferData(GL_ARRAY_BUFFER, BOX_COUNT*sizeof(BoxData), NULL, GL_STATIC_DRAW);

		//set up base quad pointer
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quadBufferID);
		glVertexAttribPointer(
			0, //layout = 0
			3, //vec3
			GL_FLOAT,
			GL_FALSE,
			0, //no need to stride
			(void*)0 //vertexes starting at 0
		);

		//set up position pointer
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, instanceBufferID);
		glVertexAttribPointer(
			1, //layout = 1
			3, //vec3
			GL_FLOAT,
			GL_FALSE,
			sizeof(BoxData), //stride the length of boxdata
			(void*)(offsetof(BoxData, BoxData::xyz)) //position part of boxdata
		);

		//set up size pointer
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, instanceBufferID);
		glVertexAttribPointer(
			2, //layout = 2
			2, //vec2
			GL_FLOAT,
			GL_FALSE,
			sizeof(BoxData), //stride the length of boxdata
			(void*)(offsetof(BoxData, BoxData::wh)) //size part of boxdata
		);

		//set up rotation pointer
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, instanceBufferID);
		glVertexAttribPointer(
			3, //layout = 3
			1, //float
			GL_FLOAT,
			GL_FALSE,
			sizeof(BoxData), //stride the length of boxdata
			(void*)(offsetof(BoxData, BoxData::rotation)) //rotation value
		);

		//Set up texture access pointer
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, instanceBufferID);
		glVertexAttribPointer(
			4, //layout = 4
			4, //vec 4 (u,v,w,l)
			GL_FLOAT,
			GL_FALSE,
			sizeof(BoxData), //stride the length of boxdata
			(void*)(offsetof(BoxData, BoxData::uvwh)) //textjre access
		);

		glVertexAttribDivisor(0, 0); //base quad
		glVertexAttribDivisor(1, 1); //position
		glVertexAttribDivisor(2, 1); //size
		glVertexAttribDivisor(3, 1); //rotation
		glVertexAttribDivisor(4, 1); //texture access

		instanceMap = (BoxData*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(instanceMap, &instanceData, boxCount * sizeof(BoxData));
		glUnmapBuffer(GL_ARRAY_BUFFER);

		//unbind VAO
		glBindVertexArray(0);
	}

	unsigned long long int getUnmapIndex() {
		return unmapIndex.load();
	}

};

#endif
