/*Indicates the entity not be
Culled when out of game bounds*/
#ifndef __COMPONENT_NO_BOUNDS_CONTROL__
#define __COMPONENT_NO_BOUNDS_CONTROL__

#include "component.h"

#include "scriptable_class.h"

class ComponentNoBoundsControl : public Component, public ScriptableClass<ComponentNoBoundsControl> {
public:
	static void registerToLua(kaguya::State& state) {
		state["ComponentNoBoundsControl"].setClass(kaguya::UserdataMetatable<ComponentNoBoundsControl, Component>()
			.setConstructors<ComponentNoBoundsControl()>()
			.addStaticFunction("create", ScriptableClass::create<ComponentNoBoundsControl>)
			.addStaticFunction("type", &getType<ComponentNoBoundsControl>)
			.addStaticFunction("cast", &Component::castDown<ComponentNoBoundsControl>)
		);
	}
};

#endif