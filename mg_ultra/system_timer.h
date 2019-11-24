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

		target = SubPool(
			SubPoolComponents::ByComponents<ComponentTimer>()
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto comText = ent->getComponent<ComponentTimer>();
		comText->updateTimer();

		auto callbacks = comText->getCallbacks(comText->getCycle());
		for (auto i : callbacks) {
			ScriptUnit su(SS_timedCallBack, i);
			su.attachEntity(ent);
			su.addDebugData(to_string(id) + " " + to_string(comText->getCycle()));
			executeScriptUnit(su);
		}

	}
};

#endif