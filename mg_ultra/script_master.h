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

	void finalExecuteScriptUnit(ScriptUnit scriptUnit) {
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

	void addScriptUnit(ScriptUnit scriptUnit);

	void executeBufferedScripts();

	//gets size of script
	int getNumberOfScripts() {
		unique_lock<mutex> lck(scriptBufferLock);
		return scriptList.size();
	}

};

//execute a script unit globally
void executeScriptUnit(ScriptUnit scriptUnit);

#endif