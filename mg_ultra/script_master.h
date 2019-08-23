/*Abstraction of lua state*/
#ifndef __SCRIPT_MASTER__
#define __SCRIPT_MASTER__

#include <mutex>

#include "scriptable_class.h"
#include "events.h"
#include "str_kit.h"

template <class T>
static type_index getType() {
	return typeid(T);
}

vector<string> pullScriptErrors();

/*Encapsulates the lua state machine
execution done by systems_masters
*/
class ScriptMaster {
	kaguya::State kaguya;

	//true when executing a script
	atomic<bool> executingScript = false;
	//script master is shut down
	atomic<bool> disabled = false;

	//locks when copying script buffer
	mutex scriptBufferLock;

	//list of script units to execute
	vector<ScriptUnit> scriptList;

	//loads and executes a script
	//use to load and initialise scripts
	void quickLoadAndExecute(string location) {
		kaguya.dofile(location.c_str());
		//check script errors
		vector<string> buffer = pullScriptErrors();
		if (buffer.size()) {
			err::logMessage("SCRIPT: Error, an error occured during the execution of a script in " + location);
			for (auto i : buffer) {
				err::logMessage(i);
			}
		}
	}

	void finalExecuteScriptUnit(ScriptUnit scriptUnit);

public:
	ScriptMaster();

	//call before destroying
	void disable();

	~ScriptMaster();

	kaguya::State& getKaguya() {
		return kaguya;
	}

	void addScriptUnit(ScriptUnit scriptUnit);

	void executeBufferedScripts();

	//gets size of script
	int getNumberOfScripts() {
		unique_lock<mutex> lck(scriptBufferLock);
		return scriptList.size();
	}
};

//closes the pipeline
void closeScriptPipeline();

//execute a script unit globally
void executeScriptUnit(ScriptUnit scriptUnit);

#endif