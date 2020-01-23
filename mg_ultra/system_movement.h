/*A system that updates entities that contain 
a component movement, and component position
Moves acceleration into velocity and velocity into
component position*/

#ifndef __SYSTEM_MOVEMENT__
#define __SYSTEM_MOVEMENT__

#include "system.h"

#include "component_position.h"
#include "component_movement.h"
#include "component_static_movement.h"
#include "component_clamp_position.h"

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
		auto sta = getComponent<ComponentStaticMovement>(components);
		auto cla = getComponent<ComponentClampPosition>(components);

		if (mov->getUpdateInSystem()) {
			Point3 currentPos = pos->getPosition3();
			if (sta) {
				currentPos = sta->getUpdatedPosition(mov, currentPos);
			}

			currentPos = mov->getUpdatedPosition(currentPos);

			if (cla) {
				currentPos = cla->applyClamp(currentPos);
			}
			
			pos->setPosition(currentPos);
			
		}
		
	}

};


#endif