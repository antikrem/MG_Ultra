/*A system that updates rotation :^(
*/

#ifndef __SYSTEM_ROTATION__
#define __SYSTEM_ROTATION__

#include "system.h"
	
#include "component_graphics.h"
#include "component_movement.h"
#include "component_rotation.h"

class SystemRotation : public System {

public:
	SystemRotation() {
		debugName = "s_rotation";
		
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentGraphics, ComponentRotation>()
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto gra = getComponent<ComponentGraphics>(components);
		auto rot = getComponent<ComponentRotation>(components);
		auto mov = getComponent<ComponentMovement>(components);

		float rotation = 0;

		if (mov && rot->isFaceMovement()) {
			rotation = mov->getDirection();
		}

		gra->setRotation(rotation);

	}

};


#endif