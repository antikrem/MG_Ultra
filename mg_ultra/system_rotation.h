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
		types.push_back(typeid(ComponentGraphics));
		types.push_back(typeid(ComponentMovement));

		requiredTypes.push_back(typeid(ComponentGraphics));
		requiredTypes.push_back(typeid(ComponentMovement));
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, int entityType, int id) override {
		auto gra = getComponent<ComponentGraphics>(components);
		auto mov = getComponent<ComponentMovement>(components);

		gra->setRotation(mov->getUpdatedRotation(gra->getRotation()));

	}

};


#endif