/*Abstraction of lua state*/
#ifndef __SCRIPT_MASTER__
#define __SCRIPT_MASTER__

#include <mutex>

#include "scriptable_class.h"
#include "events.h"

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

	//lock for script unit vector
	mutex lock;
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

	void executeScriptUnit(ScriptUnit scriptUnit) {
		ScriptSources source = scriptUnit.getSource();
		//explicit vector of shared pointers
		vector<shared_ptr<Entity>> copies;
		//create entity environment
		if (scriptUnit.numberOfAttachedEnts()) {
			copies.push_back(scriptUnit.getAttachedInt(0));
			kaguya["this"] = copies[0].get();
		}
		
		if (source == SS_commandLine) {
			kaguya.dostring(scriptUnit.getScript());
			vector<string> buffer = pullScriptErrors();
			if (buffer.size()) {
				err::logMessage("SCRIPT: Error, Last command line was invalid");
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

		//clear old environment
		if (scriptUnit.numberOfAttachedEnts()) {
			kaguya["this"] = nullptr;
		}
	}

public:
	ScriptMaster();

	kaguya::State& getKaguya() {
		return kaguya;
	}

	void addScriptUnit(ScriptUnit scriptUnit) {
		lock_guard<mutex> lck(lock);
		scriptList.push_back(scriptUnit);
	}

	void executeBufferedScriptUnits() {
		vector<ScriptUnit> buffer;
		{
			lock_guard<mutex> lck(lock);
			buffer = scriptList;
			scriptList.clear();
		}
		for (auto& i : buffer) {
			executeScriptUnit(i);
		}
	}

};

//execute a script unit globally
void executeScriptUnit(ScriptUnit scriptUnit);

#endif