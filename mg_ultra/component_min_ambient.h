#/*A Component that allows an entity to have a minimum ambient value
Great for bloom*/
#ifndef __COMPONENT_MIN_AMBIENT__
#define __COMPONENT_MIN_AMBIENT__

#include <atomic>
#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

class ComponentMinAmbient : public Component, public ScriptableClass<ComponentMinAmbient> {
private:
	atomic<float> minAmbient = 0.0f;

public:
	ComponentMinAmbient() {

	}

	ComponentMinAmbient(float minAmbient) {
		this->minAmbient = minAmbient;
	}

	void setValue(float minAmbient) {
		this->minAmbient = minAmbient;
	}

	float getValue() {
		return minAmbient;
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentMinAmbient"].setClass(kaguya::UserdataMetatable<ComponentMinAmbient, Component>()
			.setConstructors<ComponentMinAmbient()>()
			.addFunction("get", &ComponentMinAmbient::getValue)
			.addFunction("set", &ComponentMinAmbient::setValue)
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentMinAmbient>,
				ScriptableClass::create<ComponentMinAmbient, float>
			)
			.addStaticFunction("type", &getType<ComponentMinAmbient>)
			.addStaticFunction("cast", &Component::castDown<ComponentMinAmbient>)
		);
	}
};

#endif