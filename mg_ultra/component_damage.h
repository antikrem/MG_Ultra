/*A Component that is a damage value, will do damage on contact
if comes into contact with an entity */
#ifndef __COMPONENT_DAMAGE__
#define __COMPONENT_DAMAGE__

#include <atomic>
#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

class ComponentDamage : public Component, public ScriptableClass {
private:
	atomic<float> damage;

public:
	ComponentDamage() {

	}

	ComponentDamage(float damage) {
		this->damage = damage;
	}

	void setDamage(float damage) {
		this->damage = damage;
	}

	float getDamage() {
		return damage;
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentDamage"].setClass(kaguya::UserdataMetatable<ComponentDamage, Component>()
			.setConstructors<ComponentDamage()>()
			.addFunction("get_damage", &ComponentDamage::getDamage)
			.addFunction("set_damage", &ComponentDamage::setDamage)
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentDamage, float>
			)
			.addStaticFunction("type", &getType<ComponentDamage>)
			.addStaticFunction("cast", &Component::castDown<ComponentDamage>)
		);
	}
};


#endif