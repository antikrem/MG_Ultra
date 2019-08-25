/*Some systems load and execute an internal script 
This script facilitates easy use*/
#ifndef __SCRIPTING_SYSTEM__ 
#define __SCRIPTING_SYSTEM__

#include "script_master.h"
#include "entity.h"

class FunctionalCallbackSystem {
	string internalScript;

	//set a priori, invalidated on failure
	atomic<bool> validInternalScript = false;

protected:
	//use to set the camera script
	void setInternalScript(string script) {
		validInternalScript = true;
		internalScript = script;
	}

	//Executes the internal script, returns false on attempt to execute failed script
	bool executeInternalScript(string systemName, string systemErrorMessage, shared_ptr<Entity> ent) {
		//check for a valid script
		if (!validInternalScript) {
			return false;
		}

		//execute the camera script
		ScriptUnit su(SS_functionalCallBack, internalScript);
		su.addDebugData(systemName);
		su.attachEntity(ent);
		SuccessCallback sc;
		su.attachSuccessCallback(&sc);
		executeScriptUnit(su);

		if (!sc.waitForCompletion()) {
			err::logMessage(systemErrorMessage);
			validInternalScript = false;
		}
		return true;
	}

};

#endif