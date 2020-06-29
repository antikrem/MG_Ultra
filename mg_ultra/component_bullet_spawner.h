/*A component attached to bullet spawners,
used for managing time*/
#ifndef __COMPONENT_BULLET_SPAWNER__
#define __COMPONENT_BULLET_SPAWNER__

#include <atomic>
#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

class ComponentBulletSpawner : public Component, public ScriptableClass<ComponentBulletSpawner> {
private:
	string bulletMasterName;

	string parameterPack = "";

	int cycle = -1;
	int lastActiveScript = -1;

public:
	ComponentBulletSpawner() {

	}

	ComponentBulletSpawner(string bulletMasterName) {
		this->bulletMasterName = bulletMasterName;
	}

	ComponentBulletSpawner(string bulletMasterName, string parameterPack) {
		this->bulletMasterName = bulletMasterName;
		this->parameterPack = parameterPack;
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

	string getParameterPack() {
		return parameterPack;
	}

	static void registerToLua(kaguya::State& state) {
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