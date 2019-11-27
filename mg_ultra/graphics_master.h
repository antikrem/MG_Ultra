/*Wraps gl_handler and graphics state
i.e holds the state(graphics state) and the state transition(gl_handler)*/
#ifndef __GRAPHICS_MASTER__
#define __GRAPHICS_MASTER__

#include <thread>

#include "graphics_state.h"
#include "graphics_link.h"


class GraphicsMaster {
	thread* graphicsThread = nullptr;
	GraphicsState graphicsState;
	GraphicsLink graphicsLink;

public:
	GraphicsMaster(EntityPool* entityPool, Registar* registar) 
		: graphicsState(), graphicsLink(entityPool) {
		graphicsLink.startGLinking(&graphicsState, registar);

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

};


#endif