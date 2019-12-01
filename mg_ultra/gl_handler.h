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

#include "vao_boxdata.h"
#include "vao_screenbuffer.h"

#include "ambient_illumination.h"

/*Handles openGL handling
Only one thread calls openGL at a time
*/
class GLHandler {
private:
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

	//frame buffer for post effects
	FrameBuffer lightingFrameBuffer;

	//post effects frame buffer
	FrameBuffer postEffects;

	//buffer for all boxes in the render view
	VAOBoxData boxVAOBuffer;
	VAOScreenBuffer screenVAO;

	//Called before a render
	void prerender() {
		glDisable(GL_CULL_FACE);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	//Called after a render
	void postrender();

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
	}

	~GLHandler() {
		glfwDestroyWindow(window);
	}

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
		geometryFrameBuffer.initialiseFrameBuffer(gSettings, { "spriteColour", "spriteWorldPosition", "lightingSensitivity" }, true);
		lightingFrameBuffer.initialiseFrameBuffer(gSettings, { "pointLightScene" }, false);
		postEffects.initialiseFrameBuffer(gSettings, { "scene" }, false);

		shaderMaster = new ShaderMaster();

		//Load most important global mgt
		textureMaster->loadMGT("output.mgt", TA_global);

		err::logMessage("GRAPHICS: Subsystem initialised... Entering render loop");
		return EXIT_SUCCESS;
	}

	//renders everything in the render buffer
	//consitutes a full render cycle
	void render() {
		prerender();

		//FIRST PASS - render basic stuff
		shaderMaster->useShader("base");
		shaderMaster->setUniformMatrix4F("base", "MVP", camera->getVPMatrix());
		shaderMaster->setUniformMatrix4F("base", "uiMVP", camera->getUiVPMatrix());
		geometryFrameBuffer.bindBuffer();
		//process the box buffer, which renders the geometry
		//set mgt textures in shader
		textureMaster->attachTextures(shaderMaster, "base", "mgtSamplers");
		boxVAOBuffer.processGLSide();
		geometryFrameBuffer.unbindBuffer();

		//SECOND PASS - calculate individual lighting components
		//shaderMaster->useShader("ambient_lighting");
		//lightingFrameBuffer.bindBuffer();
		//shaderMaster->attachFrameBufferAsSource("ambient_lighting", &geometryFrameBuffer);
		//screenVAO.processGLSide();
		//lightingFrameBuffer.unbindBuffer();

		//THIRD PASS - combine all light values into post processing buffer
		shaderMaster->useShader("unified_lighting");
		shaderMaster->setUniformF("unified_lighting", "ambientStrength", g_ambient::getStrength());
		shaderMaster->setUniform3F("unified_lighting", "ambientColor", g_ambient::getColour().getVec3());
		postEffects.bindBuffer();
		shaderMaster->attachFrameBufferAsSource("unified_lighting", &lightingFrameBuffer);
		shaderMaster->attachFrameBufferAsSource("unified_lighting", &geometryFrameBuffer);
		screenVAO.processGLSide();
		postEffects.unbindBuffer();

		//FOURTH PASS - render buffer to screenspace
		//set the geometry frame buffer as the source
		shaderMaster->useShader("finalise");
		shaderMaster->attachFrameBufferAsSource("finalise", &postEffects);
		screenVAO.processGLSide();

		postrender();
	}

	GLFWwindow* getWindow() {
		return window;
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

	void setLastCalls(float lastCalls);

	float getLastCalls();
}

#endif