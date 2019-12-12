/*updates the player cached entity*/
#ifndef __SYSTEM_PLAYER__
#define __SYSTEM_PLAYER__

#include "system.h"

#include "component_position.h"
#include "component_graphics.h"
#include "component_input.h"
#include "component_movement.h"
#include "component_animation.h"
#include "component_extended_scripting.h"
#include "component_multi_entity.h"
#include "component_collision.h"

#include "functional_callback_system.h"

class SystemPlayer : public System, public FunctionalCallbackSystem  {
public:

	SystemPlayer() {
		debugName = "s_player";
		target = SubPoolTarget(
			ETPlayer
		);
		//load player script
		setInternalScript(debugName, os_kit::getFileAsString("scripts//player//player.lua"));
	}

	void cacheHandle(shared_ptr<Entity> ent) override {
		ent->getComponent<ComponentInput>()->update();
		sc.reset();
		executeInternalScript(
			"system_player", "PLAYER: Fatal error executing player script, player update disabled", 
			ent, 
			&sc
		);
	}

	void cacheFail(EntityPool* pool) override {
		//create the camera entity
		auto newEnt = shared_ptr<Entity>(new Entity(ETPlayer));
		auto newComponent = new ComponentPosition(0, 0, -10.0);
		newEnt->addComponent(newComponent->pullForEntity());
		auto newComponent1 = new ComponentGraphics("default");
		newComponent1->setScale(0.3f);
		newComponent1->setAnimationType(2);
		newEnt->addComponent(newComponent1->pullForEntity());
		auto newComponent2 = new ComponentAnimation();
		newComponent2->changeDefaultAnimation(2);
		newEnt->addComponent(newComponent2->pullForEntity());
		auto newComponent3 = new ComponentInput();
		newEnt->addComponent(newComponent3->pullForEntity());
		auto newComponent4 = new ComponentMovement();
		newEnt->addComponent(newComponent4->pullForEntity());
		auto newComponent5 = new ComponentExtendedScripting();
		newEnt->addComponent(newComponent5->pullForEntity());
		auto newComponent6 = new ComponentSpawner();
		newEnt->addComponent(newComponent6->pullForEntity());
		auto newComponent7 = new ComponentMultiEntity();
		newEnt->addComponent(newComponent7->pullForEntity());
		auto newComponent8 = new ComponentTimer();
		newEnt->addComponent(newComponent8->pullForEntity());
		auto newComponent9 = new ComponentCollision(10.0f);
		newEnt->addComponent(newComponent9->pullForEntity());

		//execute a script to initialise the player
		executeAnyScript(debugName, 
			os_kit::getFileAsString("scripts//player//initialise_player.lua"), 
			newEnt, 
			&sc
		);

		pool->addEnt(newEnt, true);
		err::logMessage("Player created");
	}
};

#endif