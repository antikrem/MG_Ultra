/* Handles updating transparency components
 */
#ifndef __SYSTEM_SCALE__
#define __SYSTEM_SCALE__

#include "component_scale.h"
#include "component_graphics.h"
#include "system.h"

class SystemScale : public System {
public:
	SystemScale() {
		debugName = "s_transparency";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentScale>()
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto compScale = getComponent<ComponentScale>(components);
		auto compGrap = getComponent<ComponentGraphics>(components);
		compScale->update();

		if (compGrap) {
			compGrap->setScale(compScale->getCurrent());
		}
	}

};

#endif