/*A Component that can be attached to graphics to
get more control over transparency*/
#ifndef __COMPONENT_TRANSPARENCY__
#define __COMPONENT_TRANSPARENCY__

#include <atomic>
#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

#include "math_ex.h"

/*Mostly workes by settings a target and speed
which will be tended to by the current*/
class ComponentTransparency : public Component, public ScriptableClass {
private:
	atomic<float> target = 1.0f;
	atomic<float> current = 1.0f;
	atomic<float> rate = 1.0f;

public:
	ComponentTransparency() {

	}

	//set directly
	ComponentTransparency(float value) {
		target = value;
		current = value;
	}

	//set to fade into final target from 0
	ComponentTransparency(float target, float rate) {
		current = 0;
		this->target = target;
		this->rate = rate;
	}

	//set all parameters
	ComponentTransparency(float target, float rate, float current) {
		this->current = current;
		this->target = target;
		this->rate = rate;
	}

	float getTarget() {
		return target;
	}

	void setTarget(float target) {
		this->target = target;
	}

	float getCurrent() { 
		return current; 
	}

	void setCurrent(float current) { 
		this->current = current;
	}

	float getRate() { 
		return rate; 
	}

	void setRate(float rate) { 
		this->rate = rate; 
	}

	//updates values
	void update() {
		current = math_ex::tend_to(current.load(), rate.load(), target.load());
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentTransparency"].setClass(kaguya::UserdataMetatable<ComponentTransparency, Component>()
			.setConstructors<ComponentTransparency()>()
			.addFunction("setTarget", &ComponentTransparency::setTarget)
			.addFunction("getTarget", &ComponentTransparency::getTarget)
			.addFunction("getCurrent", &ComponentTransparency::getCurrent)
			.addFunction("setCurrent", &ComponentTransparency::setCurrent)
			.addFunction("getRate", &ComponentTransparency::getRate)
			.addFunction("setRate", &ComponentTransparency::setRate)
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