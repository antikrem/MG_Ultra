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
Uses CRTP<This>
Each VAOBuffer has 2 vaos, one will be read and one write
T is  a renderable
*/
template <class This, class T>
class VAOBuffer {
private:
	//vao associated with this buffer
	GLuint vaoID;

	//mutex lock for switching thread context
	mutex lock;
	//conditional variable for blocking update side thread
	condition_variable cv;

	//buffer for renderable, will be of size maxsize
	T* buffer = nullptr;
	//size of current buffer
	int bufferSize = 0;

	//write buffer for renderable, will be of size maxsize
	//used by updateSide to give new values
	T* writeBuffer = nullptr;
	//set to true when writeBuffer has been updated
	atomic<bool> copyBuffer = false;
	//contains the size of the copy buffer
	int copyBufferSize = 0;

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
		delete buffer;
	}


public:

	//handles the process of this VAO, from the gl side
	void processGLSide() {
		//check if a copy buffer has come in
		if (copyBuffer) {
			{
				//copy over to buffer
				unique_lock<mutex> lck(lock);
				bufferSize = copyBufferSize;
				copy(writeBuffer, writeBuffer + bufferSize, buffer);
				copyBuffer = false;
			}
			//notify glUpdate to continue
			cv.notify_one();
			//move buffer data to gl
			glBindVertexArray(vaoID);
			glBindBuffer(GL_ARRAY_BUFFER, vboID);
			glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize * sizeof(T), buffer);
			glBindVertexArray(0);
		}

		//need to render eitherway
		glBindVertexArray(vaoID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
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
		unique_lock<mutex> lck(lock);
		copyBufferSize = size;
		//signal the glThread to handle
		copyBuffer = true;
		cv.wait(lck, [this] { return !copyBuffer.load(); });
	}

	int getMaxSize() {
		return maxSize;
	}

};

#endif