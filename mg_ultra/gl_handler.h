#ifndef __GL_HANDLER__
#define __GL_HANDLER__

#include "constants.h"
#include "shaders.h"
#include "camera.h"
#include "texture.h"
#include "n_buffer.h"
#include "error.h"

#include "frame_buffer.h"
#include "timed_block.h"
#include "performance_counter.h"

#include <thread>

#include "vao_boxdata.h"
#include "vao_screenbuffer.h"

/*Handles openGL handling
Only one thread calls openGL at a time
*/
class GLHandler {
private:
	atomic<bool> active = false;
	atomic<bool> ended = false;

	//An attached TimedBlock that can be activated
	TimedBlock periodBlock;
	//performance viewer
	PerformanceCounter performanceCounter;

	//gl handle thread, it will do all gl calls, containing current context
	thread* glThread = nullptr;

	GraphicsSettings* gSettings = nullptr;

	//Texture master
	AnimationsMaster* textureMaster = nullptr;

	//Controls shaders
	ShaderMaster* shaderMaster = nullptr;

	//window
	GLFWwindow* window = nullptr;

	//Handles camera
	Camera* camera = nullptr;

	//frame buffer for geometry
	FrameBuffer geometryFrameBuffer;

	//buffer for all boxes in the render view
	VAOBoxData boxVAOBuffer;
	VAOScreenBuffer screenVAO;

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

		//create frame buffers
		geometryFrameBuffer.initialiseFrameBuffer(gSettings, {"firstPassSampler"}, true);

		shaderMaster = new ShaderMaster();

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
		glDisable(GL_CULL_FACE);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		periodBlock.blockForTime();
	}

	//Called after a render
	void postrender();
	
	//renders everything in the render buffer
	void render() {
		prerender();

		//FIRST PASS - render basic stuff
		shaderMaster->useShader("base");
		glm::mat4 mvp = camera->getVPMatrix();
		shaderMaster->setUniform4f("base", "MVP", mvp);
		glm::mat4 uimvp = camera->getUiVPMatrix();
		shaderMaster->setUniform4f("base", "uiMVP", uimvp);
		geometryFrameBuffer.bindBuffer();
		//process the box buffer, which renders the geometry
		//set mgt textures in shader
		textureMaster->attachTextures(shaderMaster, "base", "mgtSamplers");
		boxVAOBuffer.processGLSide();
		geometryFrameBuffer.unbindBuffer();

		//SECOND PASS - render buffer to screenspace
		//set the geometry frame buffer as the source
		shaderMaster->useShader("finalise");
		shaderMaster->attachFrameBufferAsSource("finalise", &geometryFrameBuffer);
		screenVAO.processGLSide();

		postrender();
	}

public:
	//GL calls are done in a seperate thread
	//Requests made to gl thread will be handled async
	GLHandler(GLFWwindow *window, GraphicsSettings* gSettings, AnimationsMaster* textureMaster, Camera* camera) 
	: boxVAOBuffer(1000), periodBlock(60) {
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

	GLFWwindow* getWindow() {
		return window;
	}

	void blockUntilActive() {
		while not(active.load()) {

		}
	}

	//gets a boxdata buffer and commits the last one
	//blocks until operation is done
	BoxData* getBoxDataBuffer() {
		return boxVAOBuffer.getWriteBuffer();
	}

	//Commit current write buffer and waot until its processed
	void commitBoxDataBuffer(int boxCount) {
		boxVAOBuffer.commitBuffer(boxCount);
	}

	//gets the size of the BoxDataBuffer
	int getBoxDataBufferSize() {
		return boxVAOBuffer.getMaxSize();
	}
};

namespace gl_query {
	void setFPS(float fps);

	float getFPS();

	void setLastCalls(int lastCalls);

	int getLastCalls();
}

#endif