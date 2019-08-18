#include "script_master.h"

#include "component_animation.h"
#include "component_graphics.h"
#include "component_position.h"
#include "component_text.h"
#include "component_timer.h"

#include <mutex>
#include <condition_variable>

#include "system.h"

//lua error reporting catch, no error when empty
vector<string> luaErrorMessage;

void handleError(int errCode, const char * errorMessage) {
	luaErrorMessage.push_back(
		string(errorMessage)
	);
}

vector<string> pullScriptErrors() {
	vector<string> buffer = luaErrorMessage;
	luaErrorMessage.clear();
	return buffer;
}

//used to get a global pointer to kaguya
ScriptMaster* kaguyaPtr = nullptr;

ScriptMaster::ScriptMaster()
	: kaguya() {
	kaguya.setErrorHandler(&handleError);

	//register global funcs
	registerGlobalFunctions(kaguya);

	//load local library
	quickLoadAndExecute("scripts/library.lua");

	//Register base component
	kaguya["Component"].setClass(kaguya::UserdataMetatable<Component>()
		.setConstructors<Component()>()
		.addFunction("killEntity", &Component::killEntity)
		.addFunction("getFlag", &Component::getFlag)
		.addStaticFunction("type", &getType<Component>)
	);

	//register base entity
	kaguya["Entity"].setClass(
		kaguya::UserdataMetatable<Entity>()
		.setConstructors<Entity(int)>()
		.addFunction("getComponent", &Entity::_getComponent)
	);

	//register entity pool
	kaguya["EntityPool"].setClass(
		kaguya::UserdataMetatable<EntityPool>()
		.setConstructors<EntityPool()>()
		.addFunction("getEntByID", &EntityPool::getEnt)
		.addFunction("getEntFromCache", &EntityPool::getCachedEnt)
	);

	//register registar
	kaguya["Registar"].setClass(
		kaguya::UserdataMetatable<Registar>()
		.setConstructors<Registar()>()
		.addFunction("getInt", &Registar::noCheckGet<int>)
		.addFunction("getFloat", &Registar::noCheckGet<float>)
		.addFunction("getString", &Registar::noCheckGet<string>)
		.addFunction("getBool", &Registar::noCheckGet<bool>)
		.addOverloadedFunctions("add", &Registar::addToBase<int>, &Registar::addToBase<float>, &Registar::addToBase<string>, &Registar::addToBase<bool>)
		.addOverloadedFunctions("update", &Registar::update<int>, &Registar::update<float>, &Registar::update<string>, &Registar::update<bool>)
	);

	forceLuaRegistration<ComponentPosition>(kaguya);
	forceLuaRegistration<ComponentGraphics>(kaguya);
	forceLuaRegistration<ComponentText>(kaguya);
	forceLuaRegistration<ComponentTimer>(kaguya);

	//set contextual script functions
	kaguya["getEntityPool"] = getLastPool;
	kaguya["getGlobalRegistar"] = g_registar::getGlobalRegistar;

	quickLoadAndExecute("scripts/library_ex.lua");

	kaguyaPtr = this;
}

void ScriptMaster::addScriptUnit(ScriptUnit scriptUnit) {
	unique_lock<mutex> lck(scriptBufferLock);
	scriptList.push_back(scriptUnit);
}

void ScriptMaster::executeBufferedScripts() {
	vector<ScriptUnit> buffer;
	{
		unique_lock<mutex> lck(scriptBufferLock);
		buffer = scriptList;
		scriptList.clear();
	}
	for (auto& i : buffer) {
		finalExecuteScriptUnit(i);
	}
}

void executeScriptUnit(ScriptUnit scriptUnit) {
	kaguyaPtr->addScriptUnit(scriptUnit);
}