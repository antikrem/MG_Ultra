#ifndef __GL_HANDLER__
#define __GL_HANDLER__

#include "vao.h"
#include "constants.h"
#include "shaders.h"
#include "camera.h"
#include "texture.h"
#include "n_buffer.h"
#include "error.h"

#include <thread>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

/*Handles openGL handling
Only one thread calls openGL at a time
*/
class GLHandler {
private:
	atomic<bool> active = false;
	atomic<bool> ended = false;

	//size of buffer
	int size = 3;

	//unmapindex
	unsigned long long int unmapIndex = 0;

	//buffer of Vaos
	vector<Vao*> vaoBuffer;

	//Needs to be locked to make sure lowest and highest vao are valid
	mutex lock;

	//constantly updated pointer to lowest index
	atomic<Vao*> lowestVao = nullptr;
	//constantly updated pointer to lowest index
	atomic<Vao*> highestVao = nullptr;

	//gl handle thread, it will do all gl calls, containing current context
	thread* glThread = nullptr;

	GraphicsSettings* gSettings = nullptr;

	//Texture master
	AnimationsMaster* textureMaster = nullptr;

	//Controls shaders
	ShaderMaster* shaderMaster = nullptr;

	//window
	GLFWwindow* window = nullptr;

	//location of MVP
	GLuint mvpID;
	//Handles camera
	Camera* camera = nullptr;

	//initialises last part of gl
	int glThreadInitialise(GLFWwindow *window, GraphicsSettings* gSettings) {
		glfwMakeContextCurrent(window);

		err::logMessage("GRAPHICS: Initialising graphics subsystems...");

		glewExperimental = true;
		if (glewInit() != GLEW_OK) {
			return EXIT_FAILURE;
		}

		//depth test
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		//set up vao
		for (; unmapIndex < size; unmapIndex++) {
			vaoBuffer.push_back(new Vao(unmapIndex));
		}
		highestVao.store(vaoBuffer[0]);

		shaderMaster = new ShaderMaster();

		mvpID = glGetUniformLocation(shaderMaster->getShaderProgramID(), "MVP");
		glUniform1i(glGetUniformLocation(shaderMaster->getShaderProgramID(), "myTextureSampler"), 0);

		//Load most important global mgt
		textureMaster->loadMGT("output.mgt", TA_global);

		err::logMessage("GRAPHICS: Subsystem initialised... Entering render loop");
		return EXIT_SUCCESS;
	}

	//comparison function for updating lowest map
	static bool comp0(Vao* a, Vao* b) {
		return (a->getUnmapIndex() < b->getUnmapIndex());
	}

	//sets the lowest unmap index pointer
	void updateLowestUnmap() {
		lowestVao.store(
			*min_element(begin(vaoBuffer), end(vaoBuffer), &GLHandler::comp0)
		);
	}

	//sets the highest unmap index pointer
	void updateHighestUnmap() {
		for (int i = 0; i < size; i++) {
			//the highest unmap must be ready to draw
			if ( (vaoBuffer[i]->getUnmapIndex() > highestVao.load()->getUnmapIndex()) && 
				(vaoBuffer[i]->getState() == VS_readyDraw) ) {
				highestVao.store(vaoBuffer[i]);
			}
		}

	}

	//The process of gl handling
	void glThreadProcess(GLFWwindow *window, GraphicsSettings* gSettings) {
		glThreadInitialise(window, gSettings);

		active = true;
		while (active) {
			render();
		}
		ended = true;
	}

	//Called before a render
	void prerender() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	//Called after a render
	void postrender() {
		glfwSwapBuffers(window);
	}

	//does gl handling and sets min max vaos
	void glHandleAll() {
		for (auto i : vaoBuffer) {
			i->glHandle();
		}
		updateLowestUnmap();
		updateHighestUnmap();
	}
	
	//renders everything in the render buffer
	void render() {
		prerender();

		glm::mat4 mvp = camera->getVPMatrix();
		glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);

		glHandleAll();

		//draw highest unmap
		highestVao.load()->drawVao();

		postrender();
	}

public:
	//GL calls are done in a seperate thread
	//Requests made to gl thread will be handled async
	GLHandler(GLFWwindow *window, GraphicsSettings* gSettings, AnimationsMaster* textureMaster, Camera* camera) {
		this->window = window;
		this->gSettings = gSettings;
		this->camera = camera;
		this->textureMaster = textureMaster;
		//detach current process
		glfwMakeContextCurrent(NULL);
		glThread = new thread(&GLHandler::glThreadProcess, this, window, gSettings);
	}

	~GLHandler() {
		active = false;
		while not(ended) {

		}
		for (int i = 0; i < size; i++) {
			delete vaoBuffer[i];
		}
		glfwDestroyWindow(window);
	}

	unsigned long long int getNextUnmap() {
		return ++unmapIndex;
	}

	GLFWwindow* getWindow() {
		return window;
	}

	Vao* getLowestUnmap() {
		return lowestVao;
	}

	void blockUntilActive() {
		while not(active.load()) {

		}
	}
};



#endif