#include "script_master.h"

#include "component_animation.h"
#include "component_graphics.h"
#include "component_position.h"
#include "component_text.h"
#include "component_timer.h"
#include "component_camera.h"
#include "component_input.h"
#include "component_movement.h"
#include "component_extended_scripting.h"
#include "component_spawner.h"

#include "system.h"

//lua error reporting catch, no error when empty
vector<string> luaErrorMessage;

atomic<bool> failedLastScript = false;

void handleError(int errCode, const char * errorMessage) {
	luaErrorMessage.push_back(
		string(errorMessage)
	);
	failedLastScript = true;
}

vector<string> pullScriptErrors() {
	vector<string> buffer = luaErrorMessage;
	luaErrorMessage.clear();
	return buffer;
}

//used to get a global pointer to kaguya
ScriptMaster* globalScriptMasterPtr = nullptr;

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
		.addFunction("getComponent", &Entity::l_getComponent)
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
	forceLuaRegistration<ComponentCamera>(kaguya);
	forceLuaRegistration<ComponentInput>(kaguya);
	forceLuaRegistration<ComponentMovement>(kaguya);
	forceLuaRegistration<ComponentExtendedScripting>(kaguya);
	forceLuaRegistration<ComponentSpawner>(kaguya);

	//set contextual script functions
	kaguya["getEntityPool"] = getLastPool;
	kaguya["getGlobalRegistar"] = g_registar::getGlobalRegistar;

	quickLoadAndExecute("scripts/library_ex.lua");
	quickLoadAndExecute("scripts/pool_utils.lua");
	quickLoadAndExecute("scripts/math_utils.lua");

	globalScriptMasterPtr = this;
}

void ScriptMaster::disable() {
	disabled = true;
	//close the pipeline
	closeScriptPipeline();
}

ScriptMaster::~ScriptMaster() {
	//clear the buffer
	globalScriptMasterPtr = false;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void ScriptMaster::finalExecuteScriptUnit(ScriptUnit scriptUnit) {
	ScriptSources source = scriptUnit.getSource();

	//set last fail to false
	failedLastScript = false;

	//explicit vector of shared pointers
	vector<shared_ptr<Entity>> copies;

	//create entity environment
	int attachedEntCount = scriptUnit.numberOfAttachedEnts();
	if (attachedEntCount) {
		copies.push_back(scriptUnit.getAttachedEnt(0));
		kaguya["this"] = copies[0].get();
		for (int i = 1; i < attachedEntCount; i++) {
			kaguya["target" + to_string(i)] = copies[i].get();
		}
	}

	if (source == SS_commandLine) {
		kaguya.dostring(scriptUnit.getScript());
		vector<string> buffer = pullScriptErrors();
		if (buffer.size()) {
			err::logMessage("SCRIPT: Error, Last command line was invalid:");
			for (auto i : buffer) {
				err::logMessage(i);
			}
		}
	}
	else if (source == SS_inlineLoader) {
		kaguya.dostring(scriptUnit.getScript());
		vector<string> buffer = pullScriptErrors();
		if (buffer.size()) {
			err::logMessage("SCRIPT: Error, the load_table has erroneous inline request at" + scriptUnit.getDebugData()
				+ "\n--> Level loaded anyway ignoring line, produced error(s):");
			for (auto i : buffer) {
				err::logMessage(i);
			}
		}
	}
	else if (source == SS_file) {
		quickLoadAndExecute(scriptUnit.getScript());
	}
	else if (source == SS_timedCallBack) {
		kaguya.dostring(scriptUnit.getScript());
		vector<string> buffer = pullScriptErrors();
		if (buffer.size()) {
			vector<string> debugInformation = str_kit::splitOnToken(scriptUnit.getDebugData(), ' ');
			err::logMessage("SCRIPT: Error, The entity with id: " + debugInformation[0] + " failed a callback timed on " + debugInformation[1]
				+ "\n--> The error(s) occured are:");
			for (auto i : buffer) {
				err::logMessage(i);
			}
		}
	}
	else if (source == SS_functionalCallBack) {
		kaguya.dostring(scriptUnit.getScript());
		vector<string> buffer = pullScriptErrors();
		if (buffer.size()) {
			vector<string> debugInformation = str_kit::splitOnToken(scriptUnit.getDebugData(), ' ');
			err::logMessage("SCRIPT: Error, The system: " + debugInformation[0] + " failed a functional callback"
				+ "\n--> The error(s) occured are:");
			for (auto i : buffer) {
				err::logMessage(i);
			}
		}
	}

	//clear old environment
	if (scriptUnit.numberOfAttachedEnts()) {
		kaguya["this"] = nullptr;
	}

	if (attachedEntCount) {
		for (int i = 1; i < attachedEntCount; i++) {
			kaguya["target" + to_string(i)] = nullptr;
		}
	}

	//if script has an attached successfulCallback, set to true
	if (scriptUnit.getSuccessCallback()) {
		scriptUnit.getSuccessCallback()->setCompletion(!failedLastScript);
	}
	
}

void ScriptMaster::addScriptUnit(ScriptUnit scriptUnit) {
	unique_lock<mutex> lck(scriptBufferLock);
	scriptList.push_back(scriptUnit);
}

void ScriptMaster::executeBufferedScripts() {
	if (disabled) {
		return;
	}
	executingScript = true;
	vector<ScriptUnit> buffer;
	{
		unique_lock<mutex> lck(scriptBufferLock);
		buffer = scriptList;
		scriptList.clear();
	}
	for (auto& i : buffer) {
		finalExecuteScriptUnit(i);
	}
	executingScript = false;
}

//if true, the script pipeline is closed
atomic<bool> closedScriptPipeLine = false;

void closeScriptPipeline() {
	closedScriptPipeLine = true;
}

void executeScriptUnit(ScriptUnit scriptUnit) {
	if (closedScriptPipeLine) {
		//pipeline is closed, new script units are disposed
		if (scriptUnit.getSuccessCallback()) {
			scriptUnit.getSuccessCallback()->setCompletion(false);
		}
		return;
	}
	globalScriptMasterPtr->addScriptUnit(scriptUnit);
}