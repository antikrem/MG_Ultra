/*Make sures entities are cleared*/
#ifndef __SYSTEM_MULTI_ENT__
#define __SYSTEM_MULTI_ENT__

#include "system.h"
#include "component_multi_entity.h"

class SystemMultiEnt : public System {

public:
	SystemMultiEnt() {
		debugName = "s_multi_ent";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentMultiEntity>()
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		getComponent<ComponentMultiEntity>(components)->clearDeadEntities();
	}

};


#endif