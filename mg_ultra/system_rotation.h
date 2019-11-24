/*A system that updates rotation :^(
*/

#ifndef __SYSTEM_ROTATION__
#define __SYSTEM_ROTATION__

#include "system.h"
	
#include "component_graphics.h"
#include "component_movement.h"

class SystemRotation : public System {

public:
	SystemRotation() {
		debugName = "s_rotation";
		
		target = SubPool(
			SubPoolComponents::ByComponents<ComponentGraphics, ComponentMovement>()
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto gra = getComponent<ComponentGraphics>(components);
		auto mov = getComponent<ComponentMovement>(components);

		gra->setRotation(mov->getUpdatedRotation(gra->getRotation()));

	}

};


#endif