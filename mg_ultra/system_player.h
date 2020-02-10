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
#include "component_audio.h"
#include "component_listener.h"
#include "component_particle.h"
#include "component_force_applier.h"
#include "component_clamp_position.h"

#include "component_point_light.h"

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
		newEnt->addComponent(new ComponentPosition(0, 0, 1.0f));

		auto newComponent1 = new ComponentGraphics("player");
		newComponent1->setScale(0.3f);
		newComponent1->setAnimationType(1);
		newEnt->addComponent(newComponent1->pullForEntity());

		auto newComponent2 = new ComponentAnimation();
		newComponent2->changeDefaultAnimation(1);
		newEnt->addComponent(newComponent2->pullForEntity());

		newEnt->addComponent(new ComponentInput());
		newEnt->addComponent(new ComponentMovement());
		newEnt->addComponent(new ComponentExtendedScripting());
		newEnt->addComponent(new ComponentSpawner());
		newEnt->addComponent(new ComponentMultiEntity());
		newEnt->addComponent(new ComponentTimer());
		newEnt->addComponent(new ComponentCollision(10.0f));
		newEnt->addComponent(new ComponentAudio());
		newEnt->addComponent(new ComponentListener());
		newEnt->addComponent(new ComponentParticle("gold"));

		auto newComponent13 = new ComponentForceApplier(300.0f, 10.0f);
		newComponent13->setCutThreashold(0.5f);
		newEnt->addComponent(newComponent13->pullForEntity());

		newEnt->addComponent(new ComponentPointLight(1.0f, 0.75f, 0.05f));
		newEnt->addComponent(new ComponentClampPosition(960.0f, 540.0f));

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