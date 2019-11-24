/*A system that updates entities that contain 
a component movement, and component position
Moves acceleration into velocity and velocity into
component position*/

#ifndef __SYSTEM_MOVEMENT__
#define __SYSTEM_MOVEMENT__

#include "system.h"

#include "component_position.h"
#include "component_movement.h"

class SystemMovement : public System {

public:
	SystemMovement() {
		debugName = "s_movement";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentPosition, ComponentMovement>()
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto pos = getComponent<ComponentPosition>(components);
		auto mov = getComponent<ComponentMovement>(components);

		if (mov->getUpdateInSystem()) {
			pos->setPosition(mov->getUpdatedPosition(pos->getPosition3()));
		}
		
	}

};


#endif