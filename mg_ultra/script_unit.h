/*relates to script building block class*/
#ifndef __SCRIPT_UNIT__
#define __SCRIPT_UNIT__

#include <string>
#include <memory>
#include <vector>

#include "entity.h"
#include "success_callback.h"


using namespace std;

//enumeration of locations to load a script unit from
enum ScriptSources {
	//source is loaded direct from a file, no env
	SS_file,
	//script comes from command line, no env
	SS_commandLine,
	//script comes from system_loader, env
	SS_inlineLoader,
	//Call back associated to component_timer
	SS_timedCallBack,
	//Call back associated with a components evaluation
	SS_functionalCallBack
};

/*A single unit of executable script
Contains methods for accessing origin for debugging purposes*/
class ScriptUnit {
	ScriptSources scriptSource;
	string script;

	//can hold debug data
	string debugData = "";
	
	//can attach entities to this object
	vector<shared_ptr<Entity>> attachedEnts;

	//can add a successCallback to check script outcome
	SuccessCallback* callbackResult = nullptr;

public:
	//set script to file location to execute from file
	ScriptUnit(ScriptSources scriptSource, string script)
		: scriptSource(scriptSource), script(script) {

	}

	string getScript() {
		return script;
	}

	ScriptSources getSource() {
		return scriptSource;
	}

	//attach an ent
	void attachEntity(shared_ptr<Entity> ptr) {
		attachedEnts.push_back(ptr);
	}

	//attach a SuccessCallback
	void attachSuccessCallback(SuccessCallback* callbackResult) {
		this->callbackResult = callbackResult;
	}

	//get size of attached ents
	int numberOfAttachedEnts() {
		return attachedEnts.size();
	}

	//returns a copy of the shared pointer pointed by index
	shared_ptr<Entity>& getAttachedEnt(int i) {
		return attachedEnts[i];
	}

	//grabs a pointer to SuccessCallback, nullptr indicates no callback
	SuccessCallback* getSuccessCallback() {
		return callbackResult;
	}

	void addDebugData(string data) {
		debugData = data;
	}

	string getDebugData() {
		return debugData;
	}
};


#endif