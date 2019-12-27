/*Wraps gl_handler and graphics state
i.e holds the state(graphics state) and the state transition(gl_handler)*/
#ifndef __GRAPHICS_MASTER__
#define __GRAPHICS_MASTER__

#include <atomic>
#include <thread>

#include "graphics_state.h"
#include "graphics_link.h"


class GraphicsMaster {
	atomic<bool> active = true;
	atomic<bool> end = false;
	atomic<bool> initialised = false;

	GraphicsState graphicsState;
	GraphicsLink graphicsLink;

	//the process of rendering a single frame
	void renderCycle() {
		graphicsLink.linkGraphics();
		graphicsState.renderGL();
	}

	//full rendering process
	void renderProcess() {
		graphicsState.initialiseCallingThread();
		initialised = true;
		while (!end) {
			renderCycle();
		}
		active = false;
	}


public:
	GraphicsMaster(EntityPool* entityPool, Registar* registar) 
		: graphicsState(), graphicsLink(entityPool) {
		graphicsLink.startGLinking(&graphicsState, registar);
		//start the graphics thread
		thread graphicsThread(&GraphicsMaster::renderProcess, this);
		while (!initialised) {

		}
		graphicsThread.detach();
	}

	~GraphicsMaster() {
		end = true;
		//wait for graphics thread to end
		while (active) {

		}
	}

	Camera* getCamera() {
		return graphicsState.getCamera();
	}

	AnimationsMaster* getAnimationsMaster() {
		return graphicsState.getAnimationsMaster();
	}

	GLFWwindow* getWindow() {
		return graphicsState.getWindow();
	}

	//returns graphics state
	GraphicsState* getGraphicsState() {
		return &graphicsState;
	}

	void setParticleMaster(ParticleMaster* particleMaster) {
		graphicsLink.setParticleMaster(particleMaster);
	}

};


#endif