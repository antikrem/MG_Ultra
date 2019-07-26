/*Handles camera stuff, including the creation of a VP matrix
*/
#ifndef __CAMERA__
#define __CAMERA__

#include "graphics_settings.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
	//Projection matrix
	glm::mat4 projection = glm::mat4(1.0f);
	//View matrix
	glm::mat4 view = glm::mat4(1.0f);

	//culling distance
	atomic<float> cullingDistance = 3000.0f;

	GraphicsSettings* gSettings = nullptr;

public:
	Camera(GraphicsSettings* gSettings) {
		this->gSettings = gSettings;
	}

	glm::mat4 getVPMatrix() {
		projection = glm::perspective(glm::radians(45.0f), (float)gSettings->screenWidth / (float)gSettings->screenHeight, 0.1f, cullingDistance.load());
		view = glm::lookAt(
			glm::vec3(0, 0, 2000.0),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);

		return projection * view;
	}



};

#endif