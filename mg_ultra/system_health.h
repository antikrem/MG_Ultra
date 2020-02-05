/*handles cleaning up entities that die due to
receiving damage*/
#ifndef __SYSTEM_HEALTH__
#define __SYSTEM_HEALTH__

#include "component_health.h"
#include "component_multi_entity.h"
#include "component_die_with_master.h"

#include "system.h"

class SystemHealth : public System {
	

public:
	SystemHealth() {
		debugName = "s_health";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentHealth>()
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		if (ent->getFlag() && getComponent<ComponentHealth>(components)->getHealth() <= 0) {
			ent->killEntity();
		}
	}

};

#endif