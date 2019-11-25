#include "gl_handler.h"

void GLHandler::postrender() {
	glfwSwapBuffers(window);
	performanceCounter.increment();
	gl_query::setFPS(performanceCounter.getFPS());
}

///GL QUERY
atomic<float> glfps = 0;

void gl_query::setFPS(float fps) {
	glfps = fps;
}

float gl_query::getFPS() {
	return glfps;
}