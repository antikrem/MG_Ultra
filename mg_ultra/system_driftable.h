/*Updates driftabled*/

#ifndef __SYSTEM_DRIFTABLE__
#define __SYSTEM_DRIFTABLE__

#include "system.h"

#include "functional_callback_system.h"

#include "registar.h"

#include "component_position.h"
#include "component_driftable.h"

class SystemDriftable : public System, public FunctionalCallbackSystem {
	bool initialised = false;

	atomic<Point3> scrollSpeed;
	atomic<Point3> windSpeed;

public:
	SystemDriftable() 
		: scrollSpeed(Point3(0,0,0)), windSpeed(Point3(0, 0, 0)) {
		debugName = "s_driftable";

		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentPosition, ComponentDriftable>()
		);
	}


	void precycle(EntityPool* pool) override {
		if (!initialised) {
			sc.reset();
			executeAnyScript(debugName,
				os_kit::getFileAsString("scripts//_initialise//drift.lua"),
				nullptr,
				&sc
			);
			initialised = true;
		}

		//check g registar for these values
		float x, y, z;
		if (registar->get("drift_scrollspeed_x", &x)
			&& registar->get("drift_scrollspeed_y", &y)
			&& registar->get("drift_scrollspeed_z", &z)) {
			scrollSpeed = Point3(x, y, z);
		}
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto pos = getComponent<ComponentPosition>(components);
		auto dri = getComponent<ComponentDriftable>(components);

		if (dri->getScrolling()) {
			pos->addPosition(scrollSpeed.load());
		}

	}

};


#endif