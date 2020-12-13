/*
 * A Component that can be attached to graphics to
 * get more control over transparency
  */
#ifndef __COMPONENT_TRANSPARENCY__
#define __COMPONENT_TRANSPARENCY__

#include <atomic>
#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

#include "math_ex.h"
#include "trending_value.h"

/* Mostly workes by settings a target and speed
 * which will be tended to by the current
 */
class ComponentTransparency : public Component, public ScriptableClass<ComponentTransparency> {
private:

	TrendingValue<float> value;

public:
	ComponentTransparency() : value(1.0f, 1.0f, 1.0f) {

	}

	// Set directly
	ComponentTransparency(float value) : value(value, 1.0f, value) {

	}

	// Set to fade into final target from 0
	ComponentTransparency(float target, float rate) : value(0, rate, target) {

	}

	// Set all parameters
	ComponentTransparency(float target, float rate, float current) : value(current, rate, target) {

	}

	float getTarget() {
		return value.getTarget();
	}

	void setTarget(float target) {
		value.setTarget(target);
	}

	float getCurrent() { 
		return value.get();
	}

	void setCurrent(float current) { 
		value.set(current);
	}

	float getRate() { 
		return value.getRate();
	}

	void setRate(float rate) { 
		value.setRate(rate);
	}

	// Updates values
	void update() {
		value.update();
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentTransparency"].setClass(kaguya::UserdataMetatable<ComponentTransparency, Component>()
			.setConstructors<ComponentTransparency()>()
			.addFunction("set_target", &ComponentTransparency::setTarget)
			.addFunction("get_target", &ComponentTransparency::getTarget)
			.addFunction("get_current", &ComponentTransparency::getCurrent)
			.addFunction("set_current", &ComponentTransparency::setCurrent)
			.addFunction("get_rate", &ComponentTransparency::getRate)
			.addFunction("set_rate", &ComponentTransparency::setRate)
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentTransparency>,
				ScriptableClass::create<ComponentTransparency, float>,
				ScriptableClass::create<ComponentTransparency, float, float>,
				ScriptableClass::create<ComponentTransparency, float, float, float>
			)
			.addStaticFunction("type", &getType<ComponentTransparency>)
			.addStaticFunction("cast", &Component::castDown<ComponentTransparency>)
		);
	}

};

#endif