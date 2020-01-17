/*A system that updates point lights when position data comes in*/
#ifndef __SYSTEM_POINT_LIGHTS_UPDATE__
#define __SYSTEM_POINT_LIGHTS_UPDATE__

#include "system.h"

#include "component_position.h"
#include "component_point_light.h"

#include "graphics_state.h"

class SystemPointLightUpdate : public System {
public:
	SystemPointLightUpdate() {
		debugName = "s_point_lights_update";

		target = SubPoolTarget(
			{ SubPoolComponents::ByComponents<ComponentPointLight, ComponentPosition>() }
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		//move position data
		getComponent<ComponentPointLight>(components)->setPointLightPosition(getComponent<ComponentPosition>(components)->getPosition3());
	}

};

#endif