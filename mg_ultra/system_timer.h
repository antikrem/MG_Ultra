/*Iterates system timer*/
#ifndef __SYSTEM_TIMER__
#define __SYSTEM_TIMER__

#include "system.h"
#include "component_timer.h"

#include "script_master.h"

class SystemTimer : public System {


public:

	SystemTimer() {
		debugName = "s_timer";

		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentTimer>()
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto comTimer = ent->getComponent<ComponentTimer>();
		comTimer->updateTimer();

		// Apply callbacks
		int cycle = comTimer->getCycle();
		auto callbacks = comTimer->getCallbacks(cycle);
		for (auto i : callbacks) {
			ScriptUnit su(SS_timedCallBack, i);
			su.attachEntity(ent);
			su.addDebugData(to_string(id) + " " + to_string(comTimer->getCycle()));
			g_script::executeScriptUnit(su);
		}

		// Kill after death timer
		if (cycle >= comTimer->getKillCycle()) {
			ent->killEntity();
		}
	}
};

#endif