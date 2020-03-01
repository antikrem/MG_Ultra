/*Abstraction of lua state*/
#ifndef __SCRIPT_MASTER__
#define __SCRIPT_MASTER__

#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>

#include "performance_counter.h"
#include "events.h"
#include "str_kit.h"

#include "script_unit.h"

#define PATH_TO_AUTO_SCRIPT "scripts/_auto_run"
#define INIT_ENGINE_START "/_engine_start/"
#define INIT_ON_CYCLE "/_on_cycle/"
#define INIT_STATE_CHANGE "/_state_change/"

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
	//script master shut down has completed
	atomic<bool> finalised = false;

	//locks when copying script buffer
	mutex scriptBufferLock;
	//conditional variable for allowing script handle thread
	//to execute
	condition_variable cv;

	//queue of script units to execute
	queue<ScriptUnit> scriptQueue;

	PerformanceCounter pCounter;

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

	//load auto run scripts
	void loadAutoRunScript();

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

	//gets size of script
	int getNumberOfScripts() {
		unique_lock<mutex> lck(scriptBufferLock);
		return scriptQueue.size();
	}

	//Executes scripts in loop
	//until disabled
	void scriptHandling() {
		while (true) {
			//lock and wait for a script to come in
			vector<ScriptUnit> currentScripts;
			{
				unique_lock<mutex> lck(scriptBufferLock);
				cv.wait(lck,
					[this] {
						return this->scriptQueue.size() || this->disabled;
					}
				);
				if (disabled) {
					break;
				}
				else {
					while (scriptQueue.size()) {
						currentScripts.push_back(scriptQueue.front());
						scriptQueue.pop();
					}
				}
			}

			for (auto& i : currentScripts) {
				finalExecuteScriptUnit(i);
			}
		}
		finalised = true;
	}

	//starts a new thread to handle script execution
	//will continue until script master is disabled
	void beginScriptHandling() {
		thread scriptHandler(&ScriptMaster::scriptHandling, this);
		scriptHandler.detach();
	}

	//creates an entity to handle anonymous adding of ents
	void createAnonymousEntityHandling();

	//reports counts last second
	float getExecutionsLastSecond() {
		return pCounter.getReportedCalls();
	}

	//reports fps
	float getFPS() {
		return pCounter.getFPS();
	}
};

namespace g_script {
	//closes the pipeline
	void closeScriptPipeline();

	//execute a script unit globally
	void executeScriptUnit(ScriptUnit scriptUnit);

	//returns the last reports count of calls
	float callsLastSecond();

	//gets fps
	float FPS();
}

#endif