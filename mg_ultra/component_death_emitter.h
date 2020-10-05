#ifndef __COMPONENT_DEATH_EMITTER__
#define __COMPONENT_DEATH_EMITTER__

#include <atomic>

#include "component.h"

#include "scriptable_class.h"

class ComponentDeathEmitter : public Component, public ScriptableClass<ComponentDeathEmitter> {


public:

	ComponentDeathEmitter() {

	}


	static void registerToLua(kaguya::State& state) {
		state["ComponentDeathEmitter"].setClass(kaguya::UserdataMetatable<ComponentDeathEmitter, Component>()
			.setConstructors<ComponentDeathEmitter()>()
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentDeathEmitter>
			)
			.addStaticFunction("type", &getType<ComponentDeathEmitter>)
			.addStaticFunction("cast", &Component::castDown<ComponentDeathEmitter>)
		);
	}
};

#endif