/*Indicates the entity will drift from 
position shift/wind, */
#ifndef __COMPONENT_DRIFTABLE__
#define __COMPONENT_DRIFTABLE__

#include "component.h"

#include "scriptable_class.h"

class ComponentDriftable : public Component, public ScriptableClass {
	atomic<bool> scrolling = true;
	atomic<bool> windDrift = true;
public:
	void setScrolling(bool scrolling) {
		this->scrolling = scrolling;
	}

	bool getScrolling() {
		return scrolling;
	}

	void setWindDrift(bool windDrift) {
		this->windDrift = windDrift;
	}

	bool getWindDrift() {
		return windDrift;
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentDriftable"].setClass(kaguya::UserdataMetatable<ComponentDriftable, Component>()
			.setConstructors<ComponentDriftable()>()
			.addFunction("set_scrolling", &ComponentDriftable::setScrolling)
			.addFunction("get_scrolling", &ComponentDriftable::getScrolling)
			.addFunction("set_wind_drift", &ComponentDriftable::setWindDrift)
			.addFunction("get_wind_drift", &ComponentDriftable::getWindDrift)
			.addStaticFunction("create", ScriptableClass::create<ComponentDriftable>)
			.addStaticFunction("type", &getType<ComponentDriftable>)
			.addStaticFunction("cast", &Component::castDown<ComponentDriftable>)
		);
	}
};

#endif