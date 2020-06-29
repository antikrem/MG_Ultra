/* Abstraction of lua state
*/
#ifndef __SCRIPT_MASTER__
#define __SCRIPT_MASTER__

#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <deque>

#include "algorithm_ex.h"

#include "performance_counter.h"
#include "events.h"
#include "str_kit.h"

#include "script_unit.h"

#define PATH_TO_AUTO_SCRIPT "scripts/_auto_run"
#define INIT_ENGINE_START "/_engine_start/"
#define INIT_ON_CYCLE "/_on_cycle/"
#define INIT_STATE_CHANGE "/_state_change/"

vector<string> pullScriptErrors();

/* Encapsulates the lua state machine
   execution done by systems_masters
*/
class ScriptMaster {
	kaguya::State kaguya;

	// True when executing a script
	atomic<bool> executingScript = false;
	// Script master is shut down
	atomic<bool> disabled = false;
	// Script master shut down has completed
	atomic<bool> finalised = false;

	// Locks when copying script buffer
	mutex scriptBufferLock;
	// Conditional variable for allowing script handle thread
	// to execute
	condition_variable cv;

	// Queue of script units to execute
	deque<ScriptUnit> scriptQueue;

	PerformanceCounter pCounter;

	// Loads and executes a script
	// Use to load and initialise scripts
	void quickLoadAndExecute(string location) {
		kaguya.dofile(location.c_str());
		// Check script errors
		vector<string> buffer = pullScriptErrors();
		if (buffer.size()) {
			err::logMessage("SCRIPT: Error, an error occured during the execution of a script in " + location);
			for (auto i : buffer) {
				err::logMessage(i);
			}
		}
	}

	// Load auto run scripts
	void loadAutoRunScript();

	void finalExecuteScriptUnit(ScriptUnit scriptUnit);

public:
	ScriptMaster();

	// Call before destroying
	void disable();

	~ScriptMaster();

	kaguya::State& getKaguya() {
		return kaguya;
	}

	void addScriptUnit(ScriptUnit scriptUnit, bool priority = false);

	// Gets size of script
	int getNumberOfScripts() {
		unique_lock<mutex> lck(scriptBufferLock);
		return scriptQueue.size();
	}

	// Executes scripts in loop
	// until disabled
	void scriptHandling() {
		while (true) {
			// lock and wait for a script to come in
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
					extend(currentScripts, scriptQueue);
					scriptQueue.clear();
				}
			}

			for (auto& i : currentScripts) {
				finalExecuteScriptUnit(i);
			}
		}
		finalised = true;
	}

	// Starts a new thread to handle script execution
	// will continue until script master is disabled
	void beginScriptHandling() {
		thread scriptHandler(&ScriptMaster::scriptHandling, this);
		scriptHandler.detach();
	}

	// creates an entity to handle anonymous adding of ents
	void createAnonymousEntityHandling();

	// reports counts last second
	float getExecutionsLastSecond() {
		return pCounter.getReportedCalls();
	}

	// reports fps
	float getFPS() {
		return pCounter.getFPS();
	}
};

namespace g_script {
	// Closes the pipeline
	void closeScriptPipeline();

	// Execute a script unit globally
	void executeScriptUnit(ScriptUnit scriptUnit, bool priority = false);

	// Returns the last reports count of calls
	float callsLastSecond();

	// Gets fps
	float FPS();
}

#endif