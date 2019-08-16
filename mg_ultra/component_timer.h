/*Stores how many in game cycles/counter this has been alive for
can also attach scripts to this timer, that will be callbacked*/
#ifndef __COMPONENT_TIMER__
#define __COMPONENT_TIMER__

#include "component.h"
#include "constants.h"
#include "mmap.h"

#include <atomic>

class ComponentTimer : public Component, public ScriptableClass {
	atomic<int> cycle = -1;
	mutex lock;

	MMap<int, string> timedCallbacks;

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

	void addCallback(int cycle, string callback) {
		lock_guard<mutex> lck(lock);
		timedCallbacks[cycle].push_back(callback);
	}

	//get vector of all strings
	vector<string> getCallbacks(int cycle) {
		lock_guard<mutex> lck(lock);
		if (timedCallbacks.count(cycle)) {
			return timedCallbacks[cycle];
		}

		return vector<string>();
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentTimer"].setClass(kaguya::UserdataMetatable<ComponentTimer, Component>()
			.setConstructors<ComponentTimer()>()
			.addFunction("get_cycle", &ComponentTimer::getCycle)
			.addFunction("set_cycle", &ComponentTimer::setCycle)
			.addFunction("add_callback", &ComponentTimer::addCallback)
			.addStaticFunction("type", &getType<ComponentTimer>)
			.addStaticFunction("cast", &Component::castDown<ComponentTimer>)
		);
	}
};



#endif