/*Indicates the entity not be
Culled when out of game bounds*/
#ifndef __COMPONENT_NAME__
#define __COMPONENT_NAME__

#include "component.h"

#include "scriptable_class.h"

/**
 * Assigns a name to holding entity
 * Names are immutable
 */
class ComponentName : public Component, public ScriptableClass<ComponentName> {
	// Name of holding entity
	const string name;

public:

	ComponentName()
		: name("") {
	}

	ComponentName(string name)
		: name(name) {
	}

	string getName() {
		return name;
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentName"].setClass(kaguya::UserdataMetatable<ComponentName, Component>()
			.setConstructors<ComponentName()>()
			.addFunction("get_name", &ComponentName::getName)
			.addStaticFunction("create", ScriptableClass::create<ComponentName, string>)
			.addStaticFunction("type", &getType<ComponentName>)
			.addStaticFunction("cast", &Component::castDown<ComponentName>)
		);
	}
};

#endif