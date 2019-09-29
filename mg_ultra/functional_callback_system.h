/*Some systems load and execute an internal script 
This script facilitates easy use*/
#ifndef __SCRIPTING_SYSTEM__ 
#define __SCRIPTING_SYSTEM__

#include "script_master.h"
#include "entity.h"

class FunctionalCallbackSystem {
	//a script called and waited on 
	//on each executeInternalScript
	string internalScript;

	//set a priori, invalidated on failure
	atomic<bool> validInternalScript = false;

protected:
	//An optional script that will be executed
	//before the internal script
	void executeInitialScript(string systemName, string script, shared_ptr<Entity> ent, SuccessCallback* sc);

	//sets the source of the script to use
	void setInternalScript(string systemName, string script);

	//Executes the internal script, returns false on attempt to execute failed script
	bool executeInternalScript(string systemName, string systemErrorMessage, shared_ptr<Entity> ent, SuccessCallback* sc);

};

#endif