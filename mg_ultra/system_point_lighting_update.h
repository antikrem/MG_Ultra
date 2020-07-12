/* A system that updates point lights when position data comes in
*/
#ifndef __SYSTEM_POINT_LIGHTS_UPDATE__
#define __SYSTEM_POINT_LIGHTS_UPDATE__

#include "system.h"

#include "component_position.h"
#include "component_point_light.h"

#include "graphics_state.h"

#define POINT_LIGHT_EXTINCTION_UPDATE_TOLERENCE 0.00000001f

class SystemPointLightUpdate : public System {
	// Cached extinction range
	float extinctionRange;

	// Flag to update point light extinctions for cycle
	bool updateExtinctionRange = false;

public:
	SystemPointLightUpdate() {
		debugName = "s_point_lights_update";

		target = SubPoolTarget(
			{ SubPoolComponents::ByComponents<ComponentPointLight, ComponentPosition>() }
		);
	}

	void precycle(EntityPool* pool) {
		updateExtinctionRange = false;
		float extinctionRange = g_graphicsSettings::getPointLightExtinctionRange();
		if (!math_ex::comp(extinctionRange, this->extinctionRange, POINT_LIGHT_EXTINCTION_UPDATE_TOLERENCE)) {
			this->extinctionRange = extinctionRange;
			updateExtinctionRange = true;
		}
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto compPL = getComponent<ComponentPointLight>(components);
		
		// Move position data
		compPL->setPointLightPosition(getComponent<ComponentPosition>(components)->getPosition3());
		compPL->update();

		// If required, update extinction range
		if (updateExtinctionRange || compPL->isNew()) {
			compPL->updateExtinctionRange(extinctionRange);
		}
		
	}

};

#endif