#include "functional_callback_system.h"

void FunctionalCallbackSystem::setInternalScript(string script) {
	validInternalScript = true;
	internalScript = script;
}

bool FunctionalCallbackSystem::executeInternalScript(string systemName, string systemErrorMessage, shared_ptr<Entity> ent, SuccessCallback* sc) {
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
	executeScriptUnit(su);

	if (!sc->waitForCompletion()) {
		err::logMessage(systemErrorMessage);
		validInternalScript = false;
	}
	return true;
}