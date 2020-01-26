/*A component attached to bullet spawners,
used for managing time*/
#ifndef __COMPONENT_BULLET_SPAWNER__
#define __COMPONENT_BULLET_SPAWNER__

#include <atomic>
#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

class ComponentBulletSpawner : public Component, public ScriptableClass {
private:
	string bulletMasterName;

	int cycle = -1;
	int lastActiveScript = -1;

public:
	ComponentBulletSpawner() {

	}

	ComponentBulletSpawner(string bulletMasterName) {
		this->bulletMasterName = bulletMasterName;
	}

	int getCycle() {
		return cycle;
	}

	void incrementCycle() {
		cycle++;
	}

	int getLastActiceScript() {
		return lastActiveScript;
	}

	void setLastActiceScript(int lastActiveScript) {
		this->lastActiveScript = lastActiveScript;
	}

	string getBulletMasterName() {
		return bulletMasterName;
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentBulletSpawner"].setClass(kaguya::UserdataMetatable<ComponentBulletSpawner, Component>()
			.setConstructors<ComponentBulletSpawner()>()
			.addFunction("setLastActive", &ComponentBulletSpawner::setLastActiceScript)
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentBulletSpawner, string>
			)
			.addStaticFunction("type", &getType<ComponentBulletSpawner>)
			.addStaticFunction("cast", &Component::castDown<ComponentBulletSpawner>)
		);
	}
};


#endif