/*A system that is just for testing, does nothing important*/
#ifndef __SYSTEM_POSITION__
#define __SYSTEM_POSITION__

#include "system.h"
#include "component_position.h"

class SystemPosition : public System {
public:
	SystemPosition() {
		types.push_back(typeid(ComponentPosition));
		requiredTypes.push_back(typeid(ComponentPosition));
	}

	void handleComponentMap(map<type_index, Component*> components, int entityType, int id) override {
		ComponentPosition* position = (ComponentPosition*)components[typeid(ComponentPosition)];
		position->addPosition(Point2(0.0f, 0.0f));
	}

};

#endif