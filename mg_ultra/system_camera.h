/*Handles the console camera cached entity*/
#ifndef __SYSTEM_CAMERA__
#define __SYSTEM_CAMERA__

#include "system.h"
#include "graphics_state.h"
#include "os_kit.h"
#include "script_unit.h"
#include "component_camera.h"
#include "component_position.h"

class SystemCamera : public System {
	Camera* camera;

	string cameraScript;

	//set a priori, invalidated on failure
	atomic<bool> validCameraScript = false;

public:
	SystemCamera() {
		debugName = "s_camera";
		cachedTarget = ETCamera;
		cacheOnly = true;
		//load camera script
		setCameraScript(os_kit::getFileAsString("scripts//camera.lua"));
	}

	//use to set the camera script
	void setCameraScript(string script) {
		validCameraScript = true;
		cameraScript = script;
	}

	void cacheHandle(shared_ptr<Entity> ent) override {
		//check for a valid script
		if (!validCameraScript) {
			return;
		}

		//execute the camera script
		ScriptUnit su(SS_functionalCallBack, cameraScript);
		su.addDebugData("system_camera");
		su.attachEntity(ent);
		SuccessCallback sc;
		su.attachSuccessCallback(&sc);
		executeScriptUnit(su);

		if (!sc.waitForCompletion()) {
			err::logMessage("CAMERA: Fatal error executing camera script, camera disabled");
			validCameraScript = false;
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
		//create a console entity
		auto newEnt = new Entity(ETCamera);
		auto newComponent = new ComponentPosition(0, 0, 2000.0);
		newEnt->addComponent(newComponent->pullForEntity());
		auto newComponent1 = new ComponentCamera();
		newEnt->addComponent(newComponent1->pullForEntity());
		pool->addEnt(newEnt, true);
		err::logMessage("Camera created");
	}

	void setCamera(Camera* camera) {
		this->camera = camera;
	}

};

#endif