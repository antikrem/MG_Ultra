/*Loading is done asyncronously by the loader
Loader runs on game loop master, and uses the registar to control switching of levels
Loader interfaces with gl handler for mgt loading
Loader also has a list of entities on cycle timer
Loader loads a table from file, The file is in ent_table format

Ent_table:


---ent declaraction

1: spawn condition: Sets the spawn conditions for next entities
@cycle [int] - spawn on cycle
@counter [int] - spawn on counter
@callback [string] - spawn on an emmitted callback
@immediate - spawn now

2: ent declaration
ent [int] - ent direct
ent [string] - ent table look up

3. component adding
[Component name] ...parameters

*/
#ifndef __SYSTEM_LOADER__
#define __SYSTEM_LOADER__

#include "system.h"

#include "registar.h"
#include "os_kit.h"

class SystemLoader : public System {
	map<int, Entity*> cycleEnt;

	void deleteAllFutureEnts() {
		for (auto i : cycleEnt) {
			delete i.second;
		} 
		cycleEnt.clear();
	}

	//converts registar values to next file path, returns empty string on error
	string generateLoadTable() {
		string state;
		registar->get("next_state", &state);

		if (state == "title") {
			//check load sequence is valid

			return "sequences\\title\\load_table.txt";
		}
		else if (state == "level") {
			string campaign;
			int level;
			registar->get("next_campaign", &campaign);
			registar->get("next_level", &level);

			string path = "campaigns\\" + campaign + "\\" + to_string(level) + "\\load_table.txt";

			if not(os_kit::fileExists(path)) {
				return "";
			}
			else {
				return path;
			}
		}

		return "";
	}

	bool parseLoadTable(string filepath) {

	}

public:
	SystemLoader() {
		debugName = "s_loader";
		//empty system
		cacheOnly = true;
		cachedTarget = 0;
	}

	void precycle(EntityPool* pool) override {



		//check if loading is requested
		bool loading = false;
		registar->get("loading", &loading);
		//if not loading exit
		if not(loading) {
			return;
		}

		/*a load request has been evaluated by system_gamestate_controller, and has to be completed*/
		//delete future ents
		deleteAllFutureEnts();

		//generate path to load table based on registar
		string path = generateLoadTable();
		//empty path is an error
		if (path == "") {
			err::logMessage("LOAD: error, failed to load requested parameters for last load state event");
			return;
		}



		registar->update("loading", false);

	}

};

#endif