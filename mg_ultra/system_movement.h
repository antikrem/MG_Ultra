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
		types.push_back(typeid(ComponentPosition));
		types.push_back(typeid(ComponentMovement));

		requiredTypes.push_back(typeid(ComponentPosition));
		requiredTypes.push_back(typeid(ComponentMovement));
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, int entityType, int id) override {
		auto pos = getComponent<ComponentPosition>(components);
		auto mov = getComponent<ComponentMovement>(components);

		pos->setPosition(mov->getUpdatedPosition(pos->getPosition3()));
	}

};


#endif