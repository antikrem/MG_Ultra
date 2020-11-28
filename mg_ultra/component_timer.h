/* Definition for ComponentTimer class
 */
#ifndef __COMPONENT_TIMER__
#define __COMPONENT_TIMER__

#include "component.h"
#include "constants.h"
#include "mmap.h"

#include <atomic>

 /* Stores how many in game cycles/counter this has been alive for
  * can also attach scripts to this timer, that will be callbacked
  */
class ComponentTimer : public Component, public ScriptableClass<ComponentTimer> {
	atomic<int> cycle = -1;
	mutex lock;

	atomic<int> spamFrequency = 1;
	string spamCallback;

	MMap<int, string> timedCallbacks;

	int killCycle = INT_MAX;

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

	void addSpamCallback(string callback) {
		lock_guard<mutex> lck(lock);
		spamCallback = callback;
	}

	void addSpamCallback(string callback, int frequency) {
		lock_guard<mutex> lck(lock);
		spamCallback = callback;
		spamFrequency = frequency;
	}

	void l_addSpamCallback(string callback) {
		addSpamCallback(callback);
	}

	void ll_addSpamCallback(string callback, int frequency) {
		addSpamCallback(callback, frequency);
	}

	void setKillCycle(int killCycle) {
		this->killCycle = killCycle;
	}

	int getKillCycle() {
		return killCycle;
	}

	// Get vector of all callback string for this timer
	vector<string> getCallbacks(int cycle) {
		lock_guard<mutex> lck(lock);
		
		vector<string> ret;

		if (spamCallback.size() && !(cycle % spamFrequency)) {
			ret.push_back(spamCallback);
		}

		if (timedCallbacks.count(cycle)) {
			ret = timedCallbacks[cycle];
		}

		return ret;
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentTimer"].setClass(kaguya::UserdataMetatable<ComponentTimer, Component>()
			.setConstructors<ComponentTimer()>()
			.addFunction("get_cycle", &ComponentTimer::getCycle)
			.addFunction("set_cycle", &ComponentTimer::setCycle)
			.addFunction("add_callback", &ComponentTimer::addCallback)
			.addOverloadedFunctions(
				"add_spam_callback",
				&ComponentTimer::l_addSpamCallback,
				&ComponentTimer::ll_addSpamCallback
			)
			.addFunction("set_kill_cycle", &ComponentTimer::setKillCycle)
			.addFunction("get_kill_cycle", &ComponentTimer::getKillCycle)
			.addStaticFunction("type", &getType<ComponentTimer>)
			.addStaticFunction("create", &ScriptableClass::create<ComponentTimer>)
			.addStaticFunction("cast", &Component::castDown<ComponentTimer>)
		);
	}
};



#endif