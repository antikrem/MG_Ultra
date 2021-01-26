/*Allows the attaching of bullet masters
Which will be used */
#ifndef __COMPONENT_BULLET_MASTER__
#define __COMPONENT_BULLET_MASTER__

#include <atomic>
#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

class ComponentBulletMaster : public Component, public ScriptableClass<ComponentBulletMaster> {
private:
	string bulletMasterName;
	bool initialised = false;
	bool valid = true;

	string parameterPack = "";

	int startingTick = 0;
	int currentTick = -1;

public:
	ComponentBulletMaster() {
		valid = false;
	}

	ComponentBulletMaster(string bulletMasterName) {
		this->bulletMasterName = bulletMasterName;
	}

	ComponentBulletMaster(string bulletMasterName, int startingTick) {
		this->bulletMasterName = bulletMasterName;
		this->startingTick = startingTick;
	}

	ComponentBulletMaster(string bulletMasterName, int startingTick, string parameterPack) {
		this->bulletMasterName = bulletMasterName;
		this->startingTick = startingTick;
		this->parameterPack = parameterPack;
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

	// Set to play this bullet master
	// Does not check if a previous bullet spawners
	// Are deleted
	void resetTo(string name) {
		bulletMasterName = name;
		initialised = false;
		currentTick = -1;

		valid = true;
	}

	string getBulletMasterName() {
		return bulletMasterName;
	}

	string getBulletMasterParameterPack() {
		return parameterPack;
	}

	int incrementAndGetCurrentTick() {
		return ++currentTick;
	}

	int getStartingTick() {
		return startingTick;
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentBulletMaster"].setClass(kaguya::UserdataMetatable<ComponentBulletMaster, Component>()
			.setConstructors<ComponentBulletMaster()>()
			.addFunction("get_name", &ComponentBulletMaster::getBulletMasterName)
			.addFunction("reset_to", &ComponentBulletMaster::resetTo)
			.addOverloadedFunctions(
				"generate",
				&ScriptableClass::create<ComponentBulletMaster, string, int, string>,
				&ScriptableClass::create<ComponentBulletMaster>
			)
			.addStaticFunction("type", &getType<ComponentBulletMaster>)
			.addStaticFunction("cast", &Component::castDown<ComponentBulletMaster>)
		);
		//Additionally, add this extra function for a templated constructor
		state.dostring(
			"function ComponentBulletMaster.create(name, startingTick, ...) \n"
			"	if is_nil(name) then"
			"		return ComponentBulletMaster.generate() \n"
			"	end"
			"	if is_nil(startingTick) then startingTick = 0 end \n"
			"	first = true \n"
			"	pack = \"\" \n"
			"	\n"
			"	for i = 1, select('#', ...) do \n"
			"		if first then \n"
			"			first = false \n"
			"		else \n"
			"			pack = pack .. \", \" \n"
			"		end \n"
			"		pack = pack .. tostring(select(i, ...)) \n"
			"	end \n"
			"	return ComponentBulletMaster.generate(name, startingTick, pack) \n"
			"end"

		);
	}
};


#endif