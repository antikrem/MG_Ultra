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

*/
template <class This, class T>
class VAOBuffer {
private:
	//if false vao 0 is updating, 1 is drawing
	//if true vao 0 is drawing, 0 is updating
	bool updating = false;

	//2 VAO's ids for each vao buffer
	GLuint vaoID[2] = { 0 };



	//state of each VAO
	VaoState vaoState[2] = { VS_ready, VS_ready };

	//mutex lock for switching thread context
	mutex lock;
	//conditional variable for blocking update size thread
	condition_variable cv;

	//a pointer to a mapped buffer
	T* buffer = nullptr;

protected:
	//instance index, index corresponds to VAO
	GLuint vboID[2] = { 0 };

	//number of entities in each vao
	int sizes[2] = { 0 };
	//maximum number of boxes a VAO can handle
	const int maxSize;

	//set up default buffers
	VAOBuffer(int maxSize) : maxSize(maxSize) {
		glGenVertexArrays(2, vaoID);
		for (int i = 0; i < 2; i++) {
			glBindVertexArray(vaoID[i]);
			static_cast<This*>(this)->setUpAttribPointers(i);
		}
		glBindVertexArray(0);
	}

	//returns a pointer to mapped gl buffer of specified VAO
	void updateMappedBuffer(int i) {
		glBindVertexArray(vaoID[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vboID[i]);
		buffer = (T*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		glBindVertexArray(0);
	}

public:

	//handles the process of this VAO, from the gl side
	void processGLSide() {
		//lock and check if a graphics update thread is ready
		{
			unique_lock<mutex> l(lock);
			if (vaoState[(int)updating] == VS_ready) {
				//updating is side is waiting on an unmap
				if (buffer) { //checks if buffer has been allocated, to avoid unmaping a unmapped buffer
					glBindVertexArray(vaoID[(int)updating]);
					glBindBuffer(GL_ARRAY_BUFFER, vboID[(int)updating]);
					glUnmapBuffer(GL_ARRAY_BUFFER);
				}

				//updating side is ready to draw, this is the new drawing size
				updating = !updating;
				//set the new drawing thread 
				vaoState[(int)!updating] = VS_drawing;

				updateMappedBuffer((int)updating);
				vaoState[(int)updating] = VS_waiting;

				cv.notify_one();
			}
			
		}

		//need to render eitherway
		glBindVertexArray(vaoID[(int)!updating]);
		static_cast<This*>(this)->render(sizes[(int)!updating]);
		glBindVertexArray(0);
	}

	//handles the process of this VAO from the update side
	//takes the size of the last instance insertion
	//returns a pointer to the buffer when ready
	T* processUpdateSide(int size) {
		unique_lock<mutex> l(lock);
		//updating is done, set size on signal ready
		sizes[(int)updating] = size;
		vaoState[(int)updating] = VS_ready;
		//wait for updating to switch and VS_waiting to be set
		cv.wait(l, [this] { return (vaoState[(int)updating] == VS_waiting); });
		return buffer;
	}

	int getMaxSize() {
		return maxSize;
	}

};

#endif