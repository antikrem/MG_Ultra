/*Stores how many in game cycles/counter this has been alive for*/
#ifndef __COMPONENT_TIMER__
#define __COMPONENT_TIMER__

#include "component.h"
#include "constants.h"

#include <atomic>

class ComponentTimer : public Component, public ScriptableClass {
	atomic<int> cycle = -1;

public:
	void updateTimer() {
		cycle++;
	}

	int getCycle() {
		return cycle;
	}

	void setCycle(int cycle) {
		this->cycle = cycle;
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentTimer"].setClass(kaguya::UserdataMetatable<ComponentTimer, Component>()
			.setConstructors<ComponentTimer()>()
			.addFunction("get_cycle", &ComponentTimer::getCycle)
			.addFunction("set_cycle", &ComponentTimer::setCycle)
			.addStaticFunction("type", &getType<ComponentTimer>)
			.addStaticFunction("cast", &Component::castDown<ComponentTimer>)
		);
	}
};



#endif