/* A Component that can be attached to graphics to
 * get more control over scale
 */
#ifndef __COMPONENT_SCALE__
#define __COMPONENT_SCALE__

#include <atomic>
#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

#include "math_ex.h"
#include "trending_value.h"

/* Mostly workes by settings a target and speed
 * which will be tended to by the current
 */
class ComponentScale : public Component, public ScriptableClass<ComponentScale> {
private:

	TrendingValue<float> value;

public:
	ComponentScale() : value(1.0f, 1.0f, 1.0f) {

	}

	// Set directly
	ComponentScale(float value) : value(value, 1.0f, value) {

	}

	// Set to fade into final target from 0
	ComponentScale(float target, float rate) : value(0, rate, target) {

	}

	// Set all parameters
	ComponentScale(float target, float rate, float current) : value(current, rate, target) {

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
		state["ComponentScale"].setClass(kaguya::UserdataMetatable<ComponentScale, Component>()
			.setConstructors<ComponentScale()>()
			.addFunction("set_target", &ComponentScale::setTarget)
			.addFunction("get_target", &ComponentScale::getTarget)
			.addFunction("get_current", &ComponentScale::getCurrent)
			.addFunction("set_current", &ComponentScale::setCurrent)
			.addFunction("get_rate", &ComponentScale::getRate)
			.addFunction("set_rate", &ComponentScale::setRate)
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentScale>,
				ScriptableClass::create<ComponentScale, float>,
				ScriptableClass::create<ComponentScale, float, float>,
				ScriptableClass::create<ComponentScale, float, float, float>
			)
			.addStaticFunction("type", &getType<ComponentScale>)
			.addStaticFunction("cast", &Component::castDown<ComponentScale>)
		);
	}

};

#endif