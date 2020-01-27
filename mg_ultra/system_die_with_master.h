/*handles managing bullet spawners*/
#ifndef __SYSTEM_DIE_WITH_MASTER__
#define __SYSTEM_DIE_WITH_MASTER__

#include "component_die_with_master.h"

#include "script_master.h"

#include "system.h"

class SystemDieWithMaster : public System {

public:
	SystemDieWithMaster() {
		debugName = "s_die_with_master";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentDieWithMaster>()
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto dwm = getComponent<ComponentDieWithMaster>(components);

		dwm->update();
	}

};

#endif