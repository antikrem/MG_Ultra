/*Indicates this entity should be executed against a script
on death according on script side DeathScript*/
#ifndef __COMPONENT_DEATH_SCRIPT__
#define __COMPONENT_DEATH_SCRIPT__

#include <atomic>

#include "component.h"

#include "scriptable_class.h"

class ComponentDeathScript : public Component, public ScriptableClass<ComponentDeathScript> {


public:

	ComponentDeathScript() {

	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentDeathScript"].setClass(kaguya::UserdataMetatable<ComponentDeathScript, Component>()
			.setConstructors<ComponentDeathScript()>()
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentDeathScript>
			)
			.addStaticFunction("type", &getType<ComponentDeathScript>)
			.addStaticFunction("cast", &Component::castDown<ComponentDeathScript>)
		);
	}
};

#endif