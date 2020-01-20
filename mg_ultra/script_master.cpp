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
#include "component_multi_entity.h"
#include "component_spawner.h"
#include "component_driftable.h"
#include "component_no_bounds_control.h"
#include "component_directional_light.h"
#include "component_collision.h"
#include "component_damage.h"
#include "component_health.h"
#include "component_audio.h"
#include "component_listener.h"
#include "component_transparency.h"
#include "component_particle.h"
#include "component_force_applier.h"
#include "component_static_movement.h"
#include "component_point_light.h"
#include "component_offset_master.h"

#include "registar.h"
#include "pool.h"

#include "scriptable_class.h"

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
	quickLoadAndExecute("scripts/_library/library.lua");

	//Register base component
	kaguya["Component"].setClass(kaguya::UserdataMetatable<Component>()
		.setConstructors<Component()>()
		.addFunction("killEntity", &Component::killEntity)
		.addFunction("getFlag", &Component::getFlag)
		.addStaticFunction("type", &getType<Component>)
	);

	//register base entity
	forceLuaRegistration<Entity>(kaguya);

	//register entity pool
	forceLuaRegistration<EntityPool>(kaguya);

	//register registar
	forceLuaRegistration<Registar>(kaguya);

	forceLuaRegistration<ComponentPosition>(kaguya);
	forceLuaRegistration<ComponentGraphics>(kaguya);
	forceLuaRegistration<ComponentText>(kaguya);
	forceLuaRegistration<ComponentTimer>(kaguya);
	forceLuaRegistration<ComponentCamera>(kaguya);
	forceLuaRegistration<ComponentInput>(kaguya);
	forceLuaRegistration<ComponentMovement>(kaguya);
	forceLuaRegistration<ComponentExtendedScripting>(kaguya);
	forceLuaRegistration<ComponentMultiEntity>(kaguya);
	forceLuaRegistration<ComponentSpawner>(kaguya);
	forceLuaRegistration<ComponentDriftable>(kaguya);
	forceLuaRegistration<ComponentNoBoundsControl>(kaguya);
	forceLuaRegistration<ComponentDirectionalLight>(kaguya);
	forceLuaRegistration<ComponentCollision>(kaguya);
	forceLuaRegistration<ComponentDamage>(kaguya);
	forceLuaRegistration<ComponentHealth>(kaguya);
	forceLuaRegistration<ComponentAudio>(kaguya);
	forceLuaRegistration<ComponentListener>(kaguya);
	forceLuaRegistration<ComponentTransparency>(kaguya);
	forceLuaRegistration<ComponentParticle>(kaguya);
	forceLuaRegistration<ComponentForceApplier>(kaguya);
	forceLuaRegistration<ComponentStaticMovement>(kaguya);
	forceLuaRegistration<ComponentPointLight>(kaguya);
	forceLuaRegistration<ComponentOffsetMaster>(kaguya);

	//set contextual script functions
	kaguya["getEntityPool"] = getGlobalPool;
	kaguya["getGlobalRegistar"] = g_registar::getGlobalRegistar;

	quickLoadAndExecute("scripts/_library/library_ex.lua");
	quickLoadAndExecute("scripts/_library/pool_utils.lua");
	quickLoadAndExecute("scripts/_library/math_utils.lua");
	quickLoadAndExecute("scripts/_library/string_ex.lua");

	quickLoadAndExecute("scripts/_initialise/registar.lua");
	quickLoadAndExecute("scripts/_initialise/audio.lua");
	quickLoadAndExecute("scripts/_initialise/animations.lua");
	quickLoadAndExecute("scripts/_initialise/unified_lighting.lua");
	quickLoadAndExecute("scripts/_initialise/particles.lua");
	quickLoadAndExecute("scripts/_initialise/post_effects.lua");
	quickLoadAndExecute("scripts/_initialise/weather.lua");


	globalScriptMasterPtr = this;
}

void ScriptMaster::disable() {
	disabled = true;
	//close the pipeline
	g_script::closeScriptPipeline();
	//notify change in state
	cv.notify_one();
	//wait for finalisation
	while (!finalised) {
		pass;
	}
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

	//each source has subtle differences in execution
	vector<string> buffer;
	vector<string> debugInformation;
	switch (source) {
	case SS_commandLine:
		kaguya.dostring(scriptUnit.getScript());
		buffer = pullScriptErrors();
		if (buffer.size()) {
			err::logMessage("SCRIPT: Error, Last command line was invalid:");
			for (auto i : buffer) {
				err::logMessage(i);
			}
		}
		break;
	case SS_inlineLoader:
		kaguya.dostring(scriptUnit.getScript());
		buffer = pullScriptErrors();
		if (buffer.size()) {
			err::logMessage("SCRIPT: Error, the load_table has erroneous inline request at" + scriptUnit.getDebugData()
				+ "\n--> Level loaded anyway ignoring line, produced error(s):");
			for (auto i : buffer) {
				err::logMessage(i);
			}
		}
		break;
	case SS_file:
		quickLoadAndExecute(scriptUnit.getScript());
		break;
	case SS_timedCallBack:
		kaguya.dostring(scriptUnit.getScript());
		buffer = pullScriptErrors();
		if (buffer.size()) {
			debugInformation = str_kit::splitOnToken(scriptUnit.getDebugData(), ' ');
			err::logMessage("SCRIPT: Error, The entity with id: " + debugInformation[0] + " failed a callback timed on " + debugInformation[1]
				+ "\n--> The error(s) occured are:");
			for (auto i : buffer) {
				err::logMessage(i);
			}
		}
		break;
	case SS_functionalCallBack:
		kaguya.dostring(scriptUnit.getScript());
		buffer = pullScriptErrors();
		if (buffer.size()) {
			debugInformation = str_kit::splitOnToken(scriptUnit.getDebugData(), ' ');
			err::logMessage("SCRIPT: Error, The system: " + debugInformation[0] + " failed a functional callback"
				+ "\n--> The error(s) occured are:");
			for (auto i : buffer) {
				err::logMessage(i);
			}
		}
		break;
	default:
		pass;
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
	
	//increment counter
	pCounter.increment();
}

void ScriptMaster::addScriptUnit(ScriptUnit scriptUnit) {
	unique_lock<mutex> lck(scriptBufferLock);
	scriptQueue.push(scriptUnit);
	cv.notify_one();
}

void ScriptMaster::createAnonymousEntityHandling() {
	//create an entity for anonymous spawning
	Entity* anonymousSpawner = new Entity(ETAnonymousSpawner);
	auto cSpawner = new ComponentSpawner();
	anonymousSpawner->addComponent(cSpawner->pullForEntity());
	getGlobalPool()->addEnt(anonymousSpawner, true);
	quickLoadAndExecute("scripts/_initialise/anonymous_spawner.lua");
}


//if true, the script pipeline is closed
atomic<bool> closedScriptPipeLine = false;

void g_script::closeScriptPipeline() {
	closedScriptPipeLine = true;
}

void g_script::executeScriptUnit(ScriptUnit scriptUnit) {
	if (closedScriptPipeLine) {
		//pipeline is closed, new script units are disposed
		if (scriptUnit.getSuccessCallback()) {
			scriptUnit.getSuccessCallback()->setCompletion(false);
		}
		return;
	}
	globalScriptMasterPtr->addScriptUnit(scriptUnit);
}

float g_script::callsLastSecond() {
	return globalScriptMasterPtr->getExecutionsLastSecond();
}

float g_script::FPS() {
	return globalScriptMasterPtr->getFPS();
}