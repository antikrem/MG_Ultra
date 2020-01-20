/*Make sures entities are cleared and positions are updated*/
#ifndef __SYSTEM_MULTI_ENT__
#define __SYSTEM_MULTI_ENT__

#include "system.h"
#include "component_position.h"
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
		
		auto cMultiEnt = getComponent<ComponentMultiEntity>(components);
		auto cPos = getComponent<ComponentPosition>(components);
		//clear any dead entities
		cMultiEnt->clearDeadEntities();

		if (cPos) {
			cMultiEnt->updateOffsetSubs(cPos->getPosition3());
		}
	}

};


#endif