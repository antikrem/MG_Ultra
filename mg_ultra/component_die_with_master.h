/*Indicates a sub entity will die when the
master entity it is attached to through component_multi_ent
has recieved a death flag
Has internal flags to allow dying on a specific timeout*/
#ifndef __COMPONENT_DIE_WITH_MASTER__
#define __COMPONENT_DIE_WITH_MASTER__

#include <atomic>

#include "component.h"

#include "scriptable_class.h"

class ComponentDieWithMaster : public Component, public ScriptableClass<ComponentDieWithMaster> {
	int cycle = 0;
	atomic<bool> masterDead = false;

	atomic<int> deathStart = 0;
	atomic<int> deathEnd = INT_MAX;

	// An aditional check which will see if 
	// the master is also dead, if masterDead doesn't get set
	shared_ptr<Entity> master = nullptr;


	// Double check with master
	// Returns true if flag not set
	// Or flag is set and master is dead
	bool isMasterDeadDoubleCheck() {
		return master && !master->getFlag();
	}

public:

	ComponentDieWithMaster() {

	}

	ComponentDieWithMaster(int deathStart) {
		this->deathStart = deathStart;
	}

	ComponentDieWithMaster(int deathStart, int deathEnd) {
		this->deathStart = deathStart;
		this->deathEnd = deathEnd;
	}

	void update() {
		cycle++;
		if ((masterDead || isMasterDeadDoubleCheck())
				&& deathStart < cycle
				&& cycle < deathEnd) {
			killEntity();
		}
	}

	void disableDieWithMaster() {
		//set short death end to easily disable
		deathEnd = cycle - 1;
	}

	void setMaster(shared_ptr<Entity> master) {
		this->master = master;
	}

	void setMasterDead() {
		masterDead = true;
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentDieWithMaster"].setClass(kaguya::UserdataMetatable<ComponentDieWithMaster, Component>()
			.setConstructors<ComponentDieWithMaster()>()
			.addFunction("disable", &ComponentDieWithMaster::disableDieWithMaster)
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentDieWithMaster>,
				ScriptableClass::create<ComponentDieWithMaster, int>,
				ScriptableClass::create<ComponentDieWithMaster, int, int>
			)
			.addStaticFunction("type", &getType<ComponentDieWithMaster>)
			.addStaticFunction("cast", &Component::castDown<ComponentDieWithMaster>)
		);
	}
};

#endif