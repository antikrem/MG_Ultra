/*A simple system which executes autorun scripts*/
#ifndef __SYSTEM_AUTO_RUN__
#define __SYSTEM_AUTO_RUN__

#include "os_kit.h"

#include "system.h"
#include "script_master.h"

class SystemAutoRun : public System {
private:
	bool valid = true;
	bool initialised = false;
	string script;

public:
	SystemAutoRun() {
		debugName = "s_auto_run";
		//empty target
		target = SubPoolTarget();
	}

	//Load auto script first cycle
	void precycle(EntityPool* pool) override {
		if (!initialised) {
			string path = PATH_TO_AUTO_SCRIPT;
			path.append(INIT_ON_CYCLE);

			vector<string> scripts = os_kit::getFilesInFolder(path);
			for (auto& scriptLocation : scripts) {
				script.append(os_kit::getFileAsString(path + scriptLocation));
				script.append("\n");
			}
		}

		if (valid && script.size()) {
			ScriptUnit su(SS_inlineLoader, script);
			su.addDebugData(" in on_cycle autorun");

			sc.reset();
			su.attachSuccessCallback(&sc);
			g_script::executeScriptUnit(su);
			valid = sc.waitForCompletion();
		}
	}
};
#endif