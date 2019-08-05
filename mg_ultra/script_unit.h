/*relates to script building block class*/
#ifndef __SCRIPT_UNIT__
#define __SCRIPT_UNIT__

#include <string>
#include <memory>
#include <vector>

#include "entity.h"


using namespace std;

//enumeration of locations to load a script unit from
enum ScriptSources {
	SS_file,
	SS_commandLine
};

/*A single unit of executable script
Contains methods for accessing origin for debugging purposes*/
class ScriptUnit {
	ScriptSources scriptSource;
	string script;
	
	//can attach entities to this object
	vector<shared_ptr<Entity>> attachedEnts;

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

	//get size of attached ents
	int numberOfAttachedEnts() {
		return attachedEnts.size();
	}

	//returns a copy of the shared pointer pointed by index
	shared_ptr<Entity>& getAttachedInt(int i) {
		return attachedEnts[i];
	}

};


#endif