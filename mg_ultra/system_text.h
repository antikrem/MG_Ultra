/*Updates text components, 
specifically caching when issues occur*/
#ifndef __SYSTEM_TEXT__
#define __SYSTEM_TEXT__

#include "system.h"

#include "component_text.h"

class SystemText : public System {
	//pointer to animation state
	AnimationsMaster* animationMaster = nullptr;

public:

	SystemText() {
		debugName = "s_text";

		types.push_back(typeid(ComponentText));
		requiredTypes.push_back(typeid(ComponentText));
	}

	void setAnimationMaster(AnimationsMaster* animationMaster) {
		this->animationMaster = animationMaster;
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, int entityType, int id) override {
		auto comText = getComponent<ComponentText>(components);
		comText->textUpdate(animationMaster);
	}
};

#endif