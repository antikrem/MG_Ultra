#ifndef __SYSTEM_DIRECTIONAL_LIGHT_UPDATE__
#define __SYSTEM_DIRECTIONAL_LIGHT_UPDATE__

#include "system.h"

#include "component_directional_light.h"

class SystemDirectionalLightUpdate : public System {

public:
	SystemDirectionalLightUpdate() {
		debugName = "s_directional_lighting_update";
		//target directional entities
		target = SubPoolTarget(
			{ SubPoolComponents::ByComponents<ComponentDirectionalLight>() }
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		getComponent<ComponentDirectionalLight>(components)->update();
	}

};

#endif