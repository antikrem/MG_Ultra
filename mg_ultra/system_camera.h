/*Handles the console camera cached entity*/
#ifndef __SYSTEM_CAMERA__
#define __SYSTEM_CAMERA__

#include "system.h"
#include "graphics_state.h"
#include "os_kit.h"
#include "script_unit.h"
#include "functional_callback_system.h"

#include "component_camera.h"
#include "component_no_bounds_control.h"
#include "component_position.h"

class SystemCamera : public System, public FunctionalCallbackSystem {
public:
	SystemCamera() {
		debugName = "s_camera";
		target = SubPoolTarget(
			ETCamera
		);
		//load camera script
		setInternalScript(debugName, os_kit::getFileAsString("scripts//camera//camera.lua"));
	}

	//quickly update some settings
	void precycle(EntityPool* entityPool) override {
		g_graphicsSettings::update();
	}

	void cacheHandle(shared_ptr<Entity> ent) override {
		//check if camera script is used
		bool value;
		if (registar->get("camera_script_update", &value) && !value) {
			return;
		}
		//execute camera script
		sc.reset();
		if (!executeInternalScript("system_camera", "CAMERA: Fatal error executing camera script, camera disabled", ent, &sc)) {
			return;
		}
	}

	void cacheFail(EntityPool* pool) override {
		//create the camera entity
		auto newEnt = shared_ptr<Entity>(new Entity(ETCamera));

		newEnt->addComponent(new ComponentPosition(0, 0, -1000.0));
		newEnt->addComponent(new ComponentCamera());
		newEnt->addComponent(new ComponentNoBoundsControl());

		pool->addEnt(newEnt, true);
		err::logMessage("CAMERA: camera not found, camera created");
		executeAnyScript(debugName,
			os_kit::getFileAsString("scripts//camera//initialise_camera.lua"),
			newEnt,
			&sc
		);
	}

};

#endif