#ifndef __GL_HANDLER__
#define __GL_HANDLER__

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

#include "vao_boxdata.h"

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

	//buffer for all boxes in the render view
	VAOBoxData boxVAOBuffer;

	//initialises last part of gl
	int glThreadInitialise(GLFWwindow *window, GraphicsSettings* gSettings) {
		glfwMakeContextCurrent(window);

		err::logMessage("GRAPHICS: Initialising graphics subsystems...");

		glewExperimental = true;
		if (glewInit() != GLEW_OK) {
			return EXIT_FAILURE;
		}

		glEnableDebug();

		//depth test
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		shaderMaster = new ShaderMaster();

		mvpID = glGetUniformLocation(shaderMaster->getShaderProgramID(), "MVP");
		glUniform1i(glGetUniformLocation(shaderMaster->getShaderProgramID(), "myTextureSampler"), 0);

		//Load most important global mgt
		textureMaster->loadMGT("output.mgt", TA_global);

		err::logMessage("GRAPHICS: Subsystem initialised... Entering render loop");
		return EXIT_SUCCESS;
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
	
	//renders everything in the render buffer
	void render() {
		prerender();

		glm::mat4 mvp = camera->getVPMatrix();
		glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);

		//process the box buffer, which renders the geometry
		boxVAOBuffer.processGLSide();

		postrender();
	}

public:
	//GL calls are done in a seperate thread
	//Requests made to gl thread will be handled async
	GLHandler(GLFWwindow *window, GraphicsSettings* gSettings, AnimationsMaster* textureMaster, Camera* camera) 
	: boxVAOBuffer(1000) {
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
		glfwDestroyWindow(window);
	}

	unsigned long long int getNextUnmap() {
		return ++unmapIndex;
	}

	GLFWwindow* getWindow() {
		return window;
	}

	void blockUntilActive() {
		while not(active.load()) {

		}
	}

	//gets a boxdata buffer and commits the last one
	//blocks until operation is done
	BoxData* getBoxDataBuffer(int count) {
		return boxVAOBuffer.processUpdateSide(count);
	}

	//gets the size of the BoxDataBuffer
	int getBoxDataBufferSize() {
		return boxVAOBuffer.getMaxSize();
	}
};



#endif