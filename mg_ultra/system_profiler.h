/*
Handles simple tasks like switching levels,
iterating cycle and counter at game loop speed
*/
#ifndef __SYSTEM_PROFILER__
#define __SYSTEM_PROFILER__

#include "system.h"

#include "registar.h"

#include "functional_callback_system.h"

#include "component_text.h"
#include "component_position.h"

class SystemProfiler : public System, public FunctionalCallbackSystem {
	bool profileIniScriptLoaded = false;
public:
	SystemProfiler() {
		debugName = "s_profiler";
		//empty cache target
		cachedTarget = ETProfile;
		cacheOnly = true;
		//load camera script
		setInternalScript(debugName, os_kit::getFileAsString("scripts//_initialise//profiler.lua"));
	}

	void cacheHandle(shared_ptr<Entity> ent) override {
		//update profiling 
		bool active;
		if (registar->get("profiler_active", &active) && active) {
			//execute profile script
			sc.reset();
			if (!executeInternalScript("system_profiler", "PROFILER: Fatal error executing profile script", ent, &sc)) {
				return;
			}
		}
		else {
			ent->killEntity();
		}
	}

	void cacheFail(EntityPool* pool) override {
		if (!profileIniScriptLoaded) {
			profileIniScriptLoaded = true;
			sc.reset();
			executeAnyScript(
				"system_profiler", 
				os_kit::getFileAsString("scripts//_initialise//profiling_init.lua"), 
				nullptr, 
				&sc
			);
		}

		bool active;
		if (registar->get("profiler_active", &active) && active) {
			//create the profiler entity
			auto newEnt = shared_ptr<Entity>(new Entity(ETProfile));
			auto newComponent = new ComponentPosition(0, 0);
			newEnt->addComponent(newComponent->pullForEntity());
			auto newComponent1 = new ComponentText();
			newComponent1->setText("Console");
			newComponent1->setFont("text_consolas58");
			newComponent1->setVisible(true);
			newComponent1->setScale(0.5f);
			newEnt->addComponent(newComponent1->pullForEntity());
			pool->addEnt(newEnt, true);
		}
		
	}
};


#endif