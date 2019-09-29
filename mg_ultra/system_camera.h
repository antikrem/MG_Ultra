/*Handles the console camera cached entity*/
#ifndef __SYSTEM_CAMERA__
#define __SYSTEM_CAMERA__

#include "system.h"
#include "graphics_state.h"
#include "os_kit.h"
#include "script_unit.h"
#include "functional_callback_system.h"

#include "component_camera.h"
#include "component_position.h"

class SystemCamera : public System, public FunctionalCallbackSystem {
	Camera* camera;

public:
	SystemCamera() {
		debugName = "s_camera";
		cachedTarget = ETCamera;
		cacheOnly = true;
		//load camera script
		setInternalScript(debugName, os_kit::getFileAsString("scripts//camera//camera.lua"));
	}

	void cacheHandle(shared_ptr<Entity> ent) override {
		//execute camera script
		sc.reset();
		if (!executeInternalScript("system_camera", "CAMERA: Fatal error executing camera script, camera disabled", ent, &sc)) {
			return;
		}

		//set camera values
		Point3 eyePos = ent->getComponent<ComponentPosition>()->getPosition3();
		camera->updateCamera(
			eyePos.getVec3(),
			ent->getComponent<ComponentCamera>()->getViewTarget(eyePos),
			45.0f
		);
	}

	void cacheFail(EntityPool* pool) override {
		//create the camera entity
		auto newEnt = new Entity(ETCamera);
		auto newComponent = new ComponentPosition(0, 0, 2000.0);
		newEnt->addComponent(newComponent->pullForEntity());
		auto newComponent1 = new ComponentCamera();
		newEnt->addComponent(newComponent1->pullForEntity());
		pool->addEnt(newEnt, true);
		err::logMessage("CAMERA: camera not found, camera created");
	}

	void setCamera(Camera* camera) {
		this->camera = camera;
	}

};

#endif