/*VAOBuffer allows a program to write and read from
what appears to be a vao.

*/

#ifndef __VAO_BUFFER__
#define __VAO_BUFFER__

#include "cus_struct2.h"
#include "texture.h"

#include "_graphics_headers.h"

//VAO state
enum VaoState {
	VS_waiting, //waiting for another thread to update this
	VS_ready, //finalised updating, waiting on glside to unmap
	VS_drawing  //not so useful debug state, indicating that this vao is being drawn
};

/*
Uses CRTP<This, T>, where T is the target
T is a renderable instance containing the minimum amount
of information required to update the buffer
*/
template <class This, class T>
class VAOBuffer {
private:
	//vao associated with this buffer
	GLuint vaoID;

	//buffer for renderable, will be of size maxsize
	T* buffer = nullptr;
	//size of current buffer
	int bufferSize = 0;

	//write buffer for renderable, will be of size maxsize
	//used by updateSide to give new values
	T* writeBuffer = nullptr;

protected:
	//instance index, index corresponds to VAO
	GLuint vboID = 0;

	//maximum number of boxes a VAO can handle
	const int maxSize;

	//set up default buffers
	VAOBuffer(int maxSize) : maxSize(maxSize) {
		buffer = new T[maxSize];
		writeBuffer = new T[maxSize];
		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);
		static_cast<This*>(this)->setUpAttribPointers();
		glBindVertexArray(0);
	}

	~VAOBuffer() {
		delete[] buffer;
		delete[] writeBuffer;
	}


public:

	//handles the process of this VAO, from the gl side
	void processGLSide() {
		//copy in data
		copy(writeBuffer, writeBuffer + bufferSize, buffer);

		//move buffer data to gl
		glBindVertexArray(vaoID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize * sizeof(T), buffer);
		glBindVertexArray(0);

		glEnable(GL_DEPTH_TEST);
		//need to render eitherway
		glBindVertexArray(vaoID);
		static_cast<This*>(this)->render(bufferSize);
		glBindVertexArray(0);
	}

	//grabs a pointer to the write buffer
	T* getWriteBuffer() {
		return writeBuffer;
	}

	//commits the write buffer, then waits until
	//the commit is recieved
	//should be accessed update side
	void commitBuffer(int size) {
		bufferSize = size;
	}

	int getMaxSize() {
		return maxSize;
	}

};

#endif