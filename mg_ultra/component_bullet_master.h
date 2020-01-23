/*Allows the attaching of bullet masters
Which will be used */
/*A Component that is simply a health value, that can be access in a safe way*/
#ifndef __COMPONENT_BULLET_MASTER__
#define __COMPONENT_BULLET_MASTER__

#include <atomic>
#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

class ComponentBulletMaster : public Component, public ScriptableClass {
private:
	string bulletMasterName;

	int cycle = -1;
	int lastActiveScript = -1;

	bool valid = true;

public:
	ComponentBulletMaster() {

	}

	ComponentBulletMaster(string bulletMasterName) {
		this->bulletMasterName = bulletMasterName;
	}

	bool isInitialised() {
		return cycle >= 0;
	}

	void incrementCycle() {
		cycle++;
	}

	bool isValid() {
		return valid;
	}

	void setValid(bool valid) {
		this->valid = valid;
	}

	string getBulletMasterName() {
		return bulletMasterName;
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentBulletMaster"].setClass(kaguya::UserdataMetatable<ComponentBulletMaster, Component>()
			.setConstructors<ComponentBulletMaster()>()
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentBulletMaster, string>
			)
			.addStaticFunction("type", &getType<ComponentBulletMaster>)
			.addStaticFunction("cast", &Component::castDown<ComponentBulletMaster>)
		);
	}
};


#endif