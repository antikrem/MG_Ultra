#pragma once
/**
 * Handler for component die on cycle
 */
#ifndef __SYSTEM_DIE_ON_CYCLE__
#define __SYSTEM_DIE_ON_CYCLE__

#include "system.h"
#include "component_die_on_cycle.h"

#include "script_master.h"

class SystemDieOnCycle : public System {


public:

	SystemDieOnCycle() {
		debugName = "s_die_on_cycle";

		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentDieOnCycle>()
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto comDie = ent->getComponent<ComponentDieOnCycle>();
		comDie->update();
	}
};

#endif