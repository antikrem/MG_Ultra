/*Allows the attaching of bullet masters
Which will be used */
#ifndef __COMPONENT_BULLET_MASTER__
#define __COMPONENT_BULLET_MASTER__

#include <atomic>
#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

class ComponentBulletMaster : public Component, public ScriptableClass {
private:
	string bulletMasterName;
	bool initialised = false;
	bool valid = true;

public:
	ComponentBulletMaster() {

	}

	ComponentBulletMaster(string bulletMasterName) {
		this->bulletMasterName = bulletMasterName;
	}

	bool isInitialised() {
		return initialised;
	}

	void setInitialised() {
		initialised = true;
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