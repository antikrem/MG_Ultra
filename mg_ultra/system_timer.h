/*Iterates system timer*/
#ifndef __SYSTEM_TIMER__
#define __SYSTEM_TIMER__

#include "system.h"

class SystemTimer : public System {

#include "component_timer.h"

public:

	SystemTimer() {
		debugName = "s_timer";

		types.push_back(typeid(ComponentTimer));
		requiredTypes.push_back(typeid(ComponentTimer));
	}

	void handleComponentMap(map<type_index, Component*> components, int entityType, int id) override {
		ComponentTimer* comText = (ComponentTimer*)components[typeid(ComponentTimer)];
		comText->updateTimer();
	}
};

#endif