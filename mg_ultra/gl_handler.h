#ifndef __GL_HANDLER__
#define __GL_HANDLER__

#include "constants.h"
#include "shaders.h"
#include "camera.h"
#include "texture.h"
#include "n_buffer.h"
#include "error.h"
#include "random_ex.h"

#include "frame_buffer.h"
#include "timed_block.h"
#include "performance_counter.h"

#include "vao_boxdata.h"
#include "vao_directional_light.h"
#include "vao_point_light.h"
#include "vao_screenbuffer.h"

#include "ambient_illumination.h"

//max size for guassian blur values
//Also update gauss.frag
#define GAUSSIAN_CONSTANT_MAX_LENGTH 20
#define GAUSSIAN_SAMPLE_CUTOFF 0.0001f

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

	//frame buffer for front depth buffer
	FrameBuffer frontDepthFrameBuffer;

	//frame buffer for geometry
	FrameBuffer geometryFrameBuffer;

	//frame buffer for post effects
	FrameBuffer lightingFrameBuffer;

	//post effects frame buffer
	FrameBuffer postEffects;

	//bloom ping pong frame buffer
	FrameBuffer bloom;

	//buffer for all boxes in the render view
	VAOBoxData boxVAOBuffer;
	VAOBoxData boxUIVAOBuffer;
	VAODirectionalLight directionalLightVAOBuffer;
	VAOPointLight pointLightVAOBuffer;
	VAOScreenBuffer screenVAO;

	//number of factors to utilise
	int gaussianSamples = 0;
	//memory space for setting gaussian blur factors
	float gaussianBlurFactors[GAUSSIAN_CONSTANT_MAX_LENGTH];

	//computes new gaussianSamples
	void updateGaussianSamples() {
		rand_ex::populate_half_norm(GAUSSIAN_CONSTANT_MAX_LENGTH, gSettings->bloomDeviation, gaussianBlurFactors);
		for (
			int i = 0; 
			i < GAUSSIAN_CONSTANT_MAX_LENGTH && gaussianBlurFactors[i] > GAUSSIAN_SAMPLE_CUTOFF; 
			gaussianSamples = ++i
		);
	}

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
	: boxVAOBuffer(gSettings->countMaxSpriteBox), 
	boxUIVAOBuffer(gSettings->countMaxUIBox),
	directionalLightVAOBuffer(gSettings->countMaxDirectionalLights),
	pointLightVAOBuffer(gSettings->countMaxPointLights),
	periodBlock(gSettings->targetFPS) {
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
		frontDepthFrameBuffer.initialiseFrameBuffer(
			gSettings,
			{
				{"frontDepthBuffer", GL_R32F}
			},
			DepthAttachmentOptions::ATTACH_NONE
		);
		geometryFrameBuffer.initialiseFrameBuffer(
			gSettings,
			{ 
				{"spriteColour", GL_RGBA}, 
				{"spriteWorldPosition", GL_RGB16F}, 
				{"normals", GL_RGB16_SNORM}, 
				{"lightingSensitivity", GL_R16F},
				{"minimumAmbient", GL_R16F},
				{"frontDepthBuffer", GL_R32F}
			},
			DepthAttachmentOptions::DEPTH_32
		);
		lightingFrameBuffer.initialiseFrameBuffer(
			gSettings, 
			{ 
				{"lightScene", GL_RGBA16F} 
			},
			DepthAttachmentOptions::ATTACH_NONE
		);
		
		postEffects.initialiseFrameBuffer(
			gSettings, 
			{	
				{"scene", GL_RGBA16F},
				{"brights", GL_RGBA16F}
			},
			DepthAttachmentOptions::ATTACH_NONE
		);

		bloom.initialiseFrameBuffer(
			gSettings,
			{
				{"scene", GL_RGBA16F},
				{"brights", GL_RGBA16F}
			},
			DepthAttachmentOptions::ATTACH_NONE
		);

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

		frontDepthFrameBuffer.clearBuffer();
		postEffects.clearBuffer();
		int chain;

		for (int i = 0; i < gSettings->depthPeelingPasses; i++) {
			//FIRST PASS - render basic stuff
			shaderMaster->useShader("base");
			shaderMaster->setUniformF("base", "viewport_w", (float)gSettings->screenWidth);
			shaderMaster->setUniformF("base", "viewport_h", (float)gSettings->screenHeight);
			shaderMaster->setUniformMatrix4F("base", "MVP", camera->getVPMatrix());
			geometryFrameBuffer.bindBuffer();
			//process the box buffer, which renders the geometry
			//set mgt textures in shader
			chain = textureMaster->attachTextures(shaderMaster, "base", "mgtSamplers");
			//Attach front depth buffer
			shaderMaster->attachFrameBufferAsSource("base", &frontDepthFrameBuffer, chain);
			//render boxes
			boxVAOBuffer.processGLSide();
			geometryFrameBuffer.unbindBuffer();

			//update front depth buffer
			shaderMaster->useShader("front_depth_buffer");
			frontDepthFrameBuffer.bindNoClearBuffer();
			glEnable(GL_BLEND);
			frontDepthFrameBuffer.setBlendFunction("frontDepthBuffer", GL_MAX, GL_ONE, GL_ONE);
			shaderMaster->attachFrameBufferAsSource("front_depth_buffer", &geometryFrameBuffer);
			screenVAO.processGLSide();
			glDisable(GL_BLEND);
			frontDepthFrameBuffer.unbindBuffer();


			//SECOND PASS - calculate individual lighting components
			lightingFrameBuffer.bindBuffer();
			glEnable(GL_BLEND);

			lightingFrameBuffer.setBlendFunction("lightScene", GL_FUNC_ADD, GL_ONE, GL_ONE);
			shaderMaster->useShader("directional_lighting");
			shaderMaster->attachFrameBufferAsSource("directional_lighting", &geometryFrameBuffer);
			directionalLightVAOBuffer.processGLSide();

			glDepthMask(GL_FALSE);
			shaderMaster->useShader("point_lighting");
			shaderMaster->setUniformMatrix4F("point_lighting", "MVP", camera->getVPMatrix());
			shaderMaster->attachFrameBufferAsSource("point_lighting", &geometryFrameBuffer);
			shaderMaster->setUniformF("point_lighting", "viewport_w", (float)gSettings->screenWidth);
			shaderMaster->setUniformF("point_lighting", "viewport_h", (float)gSettings->screenHeight);
			pointLightVAOBuffer.processGLSide();
			glDepthMask(GL_TRUE);

			glDisable(GL_BLEND);
			lightingFrameBuffer.unbindBuffer();

			//THIRD PASS - combine all light values into post processing buffer
			shaderMaster->useShader("unified_lighting");
			shaderMaster->setUniformF("unified_lighting", "ambientStrength", g_ambient::getStrength());
			shaderMaster->setUniform3F("unified_lighting", "ambientColor", g_ambient::getColour().getVec3());
			
			postEffects.bindNoClearBuffer();
			glEnable(GL_BLEND);
			postEffects.setBlendFunction("scene", GL_FUNC_ADD, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
			chain = shaderMaster->attachFrameBufferAsSource("unified_lighting", &lightingFrameBuffer);
			shaderMaster->attachFrameBufferAsSource("unified_lighting", &geometryFrameBuffer, chain);
			screenVAO.processGLSide();
			glDisable(GL_BLEND);
			postEffects.unbindBuffer();
		}

		//FOURTH PASS - split brights from lows for bloom
		bloom.bindBuffer();
		shaderMaster->useShader("bloom");
		shaderMaster->attachFrameBufferAsSource("bloom", &postEffects);
		shaderMaster->setUniformF("bloom", "bloomThreshold", gSettings->bloomThreshold);
		screenVAO.processGLSide();
		bloom.unbindBuffer();

		//calculate bloom ping pong buffer 
		updateGaussianSamples();
		postEffects.bindBuffer();
		shaderMaster->useShader("gauss");
		shaderMaster->attachFrameBufferAsSource("gauss", &bloom);
		shaderMaster->setUniformI("gauss", "gaussianFactorslength", gaussianSamples);
		shaderMaster->setUniform1FV("gauss", "gaussianFactors", GAUSSIAN_CONSTANT_MAX_LENGTH, gaussianBlurFactors);
		shaderMaster->setUniform2F("gauss", "offsetDirection", glm::vec2(1.0f, 0.0f));
		screenVAO.processGLSide();
		postEffects.unbindBuffer();

		//ping pong back to other buffer
		bloom.bindBuffer();
		shaderMaster->attachFrameBufferAsSource("gauss", &postEffects);
		shaderMaster->setUniform2F("gauss", "offsetDirection", glm::vec2(0.0f, 1.0f));
		screenVAO.processGLSide();
		bloom.unbindBuffer();  

		//FIFTH PASS - render buffer to screenspace
		//set the geometry frame buffer as the source
		shaderMaster->useShader("colour_correction");
		shaderMaster->setUniformF("colour_correction", "exposure", gSettings->exposure);
		shaderMaster->attachFrameBufferAsSource("colour_correction", &bloom);
		screenVAO.processGLSide();

		

		glFlush();

		//draw UI
		shaderMaster->useShader("ui");
		shaderMaster->setUniformMatrix4F("ui", "MVP", camera->getUiVPMatrix());
		textureMaster->attachTextures(shaderMaster, "ui", "mgtSamplers");
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		boxUIVAOBuffer.processGLSide();
		glDisable(GL_BLEND);
		
		postrender();
	}

	GLFWwindow* getWindow() {
		return window;
	}

	//returns reference to BoxDataVAOBuffer
	VAOBoxData& getBoxDataBuffer() {
		return boxVAOBuffer;
	}

	//returns reference to uiBoxDataVaoBuffer
	VAOBoxData& getUIBoxDataBuffer() {
		return boxUIVAOBuffer;
	}

	//returns reference to directionalLightVAOBuffer
	VAODirectionalLight& getDirectionalLightBuffer() {
		return directionalLightVAOBuffer;
	}

	//returns reference for pointLightVAOBuffer
	VAOPointLight& getPointLightBuffer() {
		return pointLightVAOBuffer;
	}

};

namespace gl_query {
	void setFPS(float fps);

	float getFPS();

	void setLastCalls(float lastCalls);

	float getLastCalls();
}

#endif