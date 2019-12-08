/*A Component that is simply a health value, that can be access in a safe way*/
#ifndef __COMPONENT_HEALTH__
#define __COMPONENT_HEALTH__

#include <atomic>
#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

class ComponentHealth : public Component, public ScriptableClass {
private:
	atomic<float> health;

public:
	ComponentHealth() {

	}

	ComponentHealth(float health) {
		this->health = health;
	}

	void damageHealth(float damage) {
		health = health - damage;
	}

	void setHealth(float health) {
		this->health = health;
	}

	float getHealth() {
		return health;
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentHealth"].setClass(kaguya::UserdataMetatable<ComponentHealth, Component>()
			.setConstructors<ComponentHealth()>()
			.addFunction("get_health", &ComponentHealth::getHealth)
			.addFunction("set_health", &ComponentHealth::setHealth)
			.addFunction("damage_health", &ComponentHealth::damageHealth)
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentHealth, float>
			)
			.addStaticFunction("type", &getType<ComponentHealth>)
			.addStaticFunction("cast", &Component::castDown<ComponentHealth>)
		);
	}
};


#endif