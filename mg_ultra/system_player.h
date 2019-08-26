/*updates the player cached entity*/
#ifndef __SYSTEM_PLAYER__
#define __SYSTEM_PLAYER__

#include "system.h"

#include "component_position.h"
#include "component_graphics.h"
#include "component_input.h"
#include "component_movement.h"

#include "functional_callback_system.h"

class SystemPlayer : public System, public FunctionalCallbackSystem  {
public:

	SystemPlayer() {
		debugName = "s_player";
		cachedTarget = ETPlayer;
		cacheOnly = true;
		//load player script
		setInternalScript(os_kit::getFileAsString("scripts//player.lua"));
	}

	void cacheHandle(shared_ptr<Entity> ent) override {
		ent->getComponent<ComponentInput>()->update();
		executeInternalScript("system_player", "PLAYER: Fatal error executing player script, player update disabled", ent);

	}

	void cacheFail(EntityPool* pool) override {
		//create the camera entity
		auto newEnt = new Entity(ETPlayer);
		auto newComponent = new ComponentPosition(0, 0, -10.0);
		newEnt->addComponent(newComponent->pullForEntity());
		auto newComponent1 = new ComponentGraphics("default");
		newEnt->addComponent(newComponent1->pullForEntity());
		auto newComponent2 = new ComponentInput();
		newEnt->addComponent(newComponent2->pullForEntity());
		auto newComponent3 = new ComponentMovement();
		newEnt->addComponent(newComponent3->pullForEntity());
		pool->addEnt(newEnt, true);
		err::logMessage("Player created");
	}
};

#endif