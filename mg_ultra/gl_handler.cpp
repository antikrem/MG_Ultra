#include "gl_handler.h"

atomic<float> glfps = 0;

void gl_query::setFPS(float fps) {
	glfps = fps;
}

float gl_query::getFPS() {
	return glfps;
}