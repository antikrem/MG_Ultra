#ifndef __SYSTEM_BACKGROUND__
#define __SYSTEM_BACKGROUND__

#include "system.h"
#include "component_multi_entity.h"
#include "component_spawner.h"

#include "functional_callback_system.h"

class SystemBackground : public System, public FunctionalCallbackSystem {

public:
	SystemBackground() {
		debugName = "s_background";
		target = SubPoolTarget(
			ETBackgroundMaster
		);

		//load background update script
		setInternalScript(debugName, os_kit::getFileAsString("scripts//backgrounds//background_master.lua"));
	}

	void cacheHandle(shared_ptr<Entity> ent) override {
		sc.reset();
		executeInternalScript(
			"system_background", "BACKGROUND: Fatal error executing background update script",
			ent,
			&sc
		);
	}

	void cacheFail(EntityPool* pool) override {
		//create the camera entity
		auto newEnt = shared_ptr<Entity>(new Entity(ETBackgroundMaster));
		auto newComponent = new ComponentMultiEntity();
		newEnt->addComponent(newComponent->pullForEntity());
		auto newComponent1 = new ComponentSpawner();
		newEnt->addComponent(newComponent1->pullForEntity());
		
		//execute a script to initialise the player
		executeAnyScript(debugName,
			os_kit::getFileAsString("scripts//backgrounds//initialise_background_master.lua"),
			newEnt,
			&sc
		);

		pool->addEnt(newEnt, true);
		err::logMessage("Created BackgroundMaster");
	}
};

#endif