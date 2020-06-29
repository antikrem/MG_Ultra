/*Allows a component */
#ifndef __COMPONENT_SPAWN_PROTECTION__
#define __COMPONENT_SPAWN_PROTECTION__

#include <atomic>
#include "component.h"
#include "constants.h"
#include "scriptable_class.h"


class ComponentSpawnProtection : public Component, public ScriptableClass<ComponentSpawnProtection> {
private:
	atomic<bool> awaitingDeath = false;
	atomic<int> cycle = -1;
	atomic<int> spawnProtectionFrames = INT_MAX;

public:
	ComponentSpawnProtection() {

	}

	ComponentSpawnProtection(int spawnProtectionFrames) {
		this->spawnProtectionFrames = spawnProtectionFrames;
	}

	void setFrames(int spawnProtectionFrames) {
		this->spawnProtectionFrames = spawnProtectionFrames;
	}

	int getFrames() {
		return spawnProtectionFrames;
	}

	void update() {
		cycle++;
	}

	void markDeathOccured() {
		awaitingDeath = true;
	}

	bool isMarkedDeathAwaiting() {
		return awaitingDeath;
	}

	bool readyToDie() {
		return awaitingDeath && cycle >= spawnProtectionFrames;
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentSpawnProtection"].setClass(kaguya::UserdataMetatable<ComponentSpawnProtection, Component>()
			.setConstructors<ComponentSpawnProtection()>()
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentSpawnProtection>,
				ScriptableClass::create<ComponentSpawnProtection, int>
			)
			.addStaticFunction("type", &getType<ComponentSpawnProtection>)
			.addStaticFunction("cast", &Component::castDown<ComponentSpawnProtection>)
		);
	}
};


#endif