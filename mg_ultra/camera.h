/*Handles camera stuff, including the creation of a VP matrix
*/
#ifndef __CAMERA__
#define __CAMERA__

#include "graphics_settings.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <atomic>

class Camera {

	//view 
	atomic<glm::mat4> projection = glm::mat4(1.0f);
	//projections
	atomic<glm::mat4> view = glm::mat4(1.0f);

	//culling distance
	atomic<float> cullingDistance = 3000.0f;

	GraphicsSettings* gSettings = nullptr;

public:
	Camera(GraphicsSettings* gSettings) {
		this->gSettings = gSettings;
		projection = glm::perspective(glm::radians(45.0f), (float)gSettings->screenWidth / (float)gSettings->screenHeight, 0.1f, cullingDistance.load());
		view = glm::lookAt(
			glm::vec3(0, 0, 2000.0),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);
	}

	void updateCamera(glm::vec3 eyePos, glm::vec3 lookAt, float fov) {
		projection = glm::perspective(
			glm::radians(fov), (float)gSettings->screenWidth / (float)gSettings->screenHeight, 0.1f, cullingDistance.load()
		);
		view = glm::lookAt(
			eyePos,
			lookAt,
			glm::vec3(0, 1, 0)
		);
	}

	glm::mat4 getVPMatrix() {
		return projection.load() * view.load();
	}



};

#endif