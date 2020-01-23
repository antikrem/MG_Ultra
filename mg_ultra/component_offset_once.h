/*Similar to offset master
but only works once for the frame the sub entity is added*/
#ifndef __COMPONENT_OFFSET_ONCE__
#define __COMPONENT_OFFSET_ONCE__

#include <atomic>

#include "cus_struct2.h"

#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

class ComponentOffsetOnce : public Component, public ScriptableClass {
private:

public:
	ComponentOffsetOnce() {

	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentOffsetOnce"].setClass(kaguya::UserdataMetatable<ComponentOffsetOnce, Component>()
			.setConstructors<ComponentOffsetOnce()>()
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentOffsetOnce>
			)
			.addStaticFunction("type", &getType<ComponentOffsetOnce>)
			.addStaticFunction("cast", &Component::castDown<ComponentOffsetOnce>)
		);
	}
};

#endif