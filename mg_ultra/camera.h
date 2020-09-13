/*Handles camera stuff, including the creation of a VP matrix
*/
#ifndef __CAMERA__
#define __CAMERA__

#include "graphics_settings.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <atomic>



class Camera {
	// Worldspace eye position
	atomic<glm::vec3> eyePos = glm::vec3(0);

	// View 
	atomic<glm::mat4> inWorldView = glm::mat4(1.0f);

	// Projections
	atomic<glm::mat4> inWorldProjection = glm::mat4(1.0f);

	// UI view
	atomic<glm::mat4> uiView = glm::mat4(1.0f);

	// UI projection
	atomic<glm::mat4> uiProjection = glm::mat4(1.0f);

	// Culling distances
	atomic<float> clipNear = 10.0f;
	atomic<float> clipFar = 4000.0f;

	// FOV
	atomic<float> fov = 45.0f;

	GraphicsSettings* gSettings = nullptr;

public:
	Camera(GraphicsSettings* gSettings) {
		this->gSettings = gSettings;

		inWorldView = glm::lookAt(
			glm::vec3(0, 0, 0),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);

		inWorldProjection = glm::perspective(
			glm::radians(45.0f), 
			(float)gSettings->screenWidth / (float)gSettings->screenHeight, 
			clipNear.load(), 
			clipFar.load()
		);
		
		uiView = glm::lookAt(
			glm::vec3(0, 0, 0),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);

		uiProjection = glm::ortho(
			-(float)1920 / 2,
			(float)1920 / 2,
			-(float)1080 / 2,
			(float)1080 / 2,
			0.1f, clipFar.load());

	}

	void updateCamera(glm::vec3 eyePos, glm::vec3 lookAt) {
		eyePos.z = -1 * eyePos.z;

		this->eyePos = eyePos;

		inWorldProjection = glm::perspective(
			glm::radians(fov.load()), 
			(float)gSettings->screenWidth / (float)gSettings->screenHeight, 
			clipNear.load(), 
			clipFar.load()
		);
		inWorldView = glm::lookAt(
			eyePos,
			lookAt,
			glm::vec3(0, 1, 0)
		);
		uiView = glm::lookAt(
			glm::vec3(0, 0, eyePos.z),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);
	}

	glm::mat4 getVPMatrix() {
		return inWorldProjection.load() * inWorldView.load();
	}

	glm::mat4 getUiVPMatrix() {
		return uiProjection.load() * uiView.load();
	}

	glm::vec3 getEyePos() {
		return eyePos;
	}

	//gets near clip distance
	float getClipNear() {
		return clipNear;
	}

	//sets near clip distance
	void setClipNear(float clipNear) {
		this->clipNear =  clipNear;
	}

	//gets far clip distance
	float getClipFar() {
		return clipFar;
	}

	//sets far clip distance
	void setClipFar(float clipFar) {
		this->clipFar = clipFar;
	}

	void setFOV(float fov) {
		this->fov = fov;
	}

	float getFOV() {
		return fov;
	}

};

#endif