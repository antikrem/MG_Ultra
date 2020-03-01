#include "functional_callback_system.h"

void FunctionalCallbackSystem::executeAnyScript(string systemName, string script, shared_ptr<Entity> ent, SuccessCallback* sc) {
	ScriptUnit su(SS_functionalCallBack, script);
	su.addDebugData(systemName);

	if (ent) {
		su.attachEntity(ent);
	}

	if (sc) {
		sc->reset();
		su.attachSuccessCallback(sc);
	}
	
	g_script::executeScriptUnit(su);

	if (sc && !sc->waitForCompletion()) {
		err::logMessage("Setup script failed in " + systemName);
	}
}

void FunctionalCallbackSystem::setInternalScript(string systemName, string script) {
	//if script is empty, report
	if (!script.size()) {
		err::logMessage("FUNCTIONAL CALLBACK: System recieved no initial script: " + systemName + ", which might be an error");
		validInternalScript = false;
		return;
	}
	validInternalScript = true;
	internalScript = script;
}

bool FunctionalCallbackSystem::executeInternalScript(string systemName, string systemErrorMessage, shared_ptr<Entity> ent, SuccessCallback* sc, bool priority) {
	//check for a valid script
	if (!validInternalScript) {
		return false;
	}

	//execute the camera script
	ScriptUnit su(SS_functionalCallBack, internalScript);
	su.addDebugData(systemName);
	su.attachEntity(ent);
	sc->reset();
	su.attachSuccessCallback(sc);
	g_script::executeScriptUnit(su, priority);

	if (!sc->waitForCompletion()) {
		err::logMessage(systemErrorMessage);
		validInternalScript = false;
	}
	return true;
}