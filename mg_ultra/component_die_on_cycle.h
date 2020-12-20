/* Declaration for ComponentDieOnCycle
 */
#ifndef __COMPONENT_DIE_ON_CYCLE__
#define __COMPONENT_DIE_ON_CYCLE__

#include "component.h"
#include "constants.h"
#include "scriptable_class.h"



 /**
  * Specifies this entity to die on a cycle
  */
class ComponentDieOnCycle : public Component, public ScriptableClass<ComponentDieOnCycle> {
	atomic<int> cycle = -1;

	atomic<int> targetCycle = INT_MAX;


public:
	ComponentDieOnCycle() {

	}

	ComponentDieOnCycle(int targetCycle) {
		this->targetCycle = targetCycle;
	}

	void update() {
		cycle++;

		if (cycle >= targetCycle) {
			this->killEntity();
		}
	}

	void setDeathCycle(int targetCycle) {
		this->targetCycle = targetCycle;
	}

	int getDeathCycle() {
		return targetCycle;
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentDieOnCycle"].setClass(kaguya::UserdataMetatable<ComponentDieOnCycle, Component>()
			.setConstructors<ComponentDieOnCycle()>()
			.addFunction("set_death_cycle", &ComponentDieOnCycle::setDeathCycle)
			.addFunction("get_death_cycle", &ComponentDieOnCycle::getDeathCycle)
			.addOverloadedFunctions(
				"create",
				&ScriptableClass::create<ComponentDieOnCycle>,
				&ScriptableClass::create<ComponentDieOnCycle, int>
			)
			.addStaticFunction("type", &getType<ComponentDieOnCycle>)
			.addStaticFunction("cast", &Component::castDown<ComponentDieOnCycle>)
		);
	}
};



#endif