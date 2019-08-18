/*Allows scripts to be executed
There is no need for components to use this system
This has no components or entity associated
*/
#ifndef __SYSTEM_SCRIPT__
#define __SYSTEM_SCRIPT__

#include "system.h"
#include "script_master.h"

class SystemScript : public System {
	ScriptMaster* scriptMaster = nullptr;

public:
	SystemScript() {
		cacheOnly = true;
		debugName = "s_script";
	}

	bool executeSystem() override {
		return scriptMaster->getNumberOfScripts();
	}

	void setScriptMaster(ScriptMaster* scriptMaster) {
		this->scriptMaster = scriptMaster;
	}

	void precycle(EntityPool* pool) override {
		scriptMaster->executeBufferedScripts();
	}
};

#endif