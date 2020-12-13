/* Handles updating transparency components
 */
#ifndef __SYSTEM_TRANSPARENCY__
#define __SYSTEM_TRANSPARENCY__

#include "component_transparency.h"
#include "system.h"

class SystemTransparency : public System {
public:
	SystemTransparency() {
		debugName = "s_transparency";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentTransparency>()
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		getComponent<ComponentTransparency>(components)->update();
	}

};

#endif