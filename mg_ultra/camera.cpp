#include "camera.h"

atomic<float> gFOV = 0.0f;

void g_camera::setFOV(float value) {
	gFOV = value;
}

float g_camera::getFOV() {
	return gFOV;
}