#include "gl_handler.h"

void GLHandler::postrender() {
	glfwSwapBuffers(window);
	performanceCounter.increment();
	gl_query::setFPS(performanceCounter.getFPS());
	gl_query::setLastCalls(performanceCounter.getReportedCalls());
}

///GL QUERY
atomic<float> glfps = 0;
atomic<float> glLastCalls = 0;

void gl_query::setFPS(float fps) {
	glfps = fps;
}

float gl_query::getFPS() {
	return glfps;
}

void gl_query::setLastCalls(float lastCalls) {
	glLastCalls = lastCalls;
}

float gl_query::getLastCalls() {
	return glLastCalls;
}