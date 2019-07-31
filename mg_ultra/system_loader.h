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

enum SL_LT_Target {
	SL_LT_cycle,
	SL_LT_counter,
	SL_LT_callback,
	SL_LT_immediate,
};

class SystemLoader : public System {
	map<int, Entity*> cycleEnt;

	//transfers ents from 
	void transferBufferedEntsToGame(EntityPool* pool) {
		//get cycle
		int cycle;
		registar->get("cycle", &cycle);
		
		//get ents equal to cycle
		for (auto it = cycleEnt.begin(); it != cycleEnt.end() && it->first < cycle; it = cycleEnt.begin()) {
			pool->addEnt(it->second);
			cycleEnt.erase(it);
		}
	}

	//clears all ents in future queue
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
			if not(os_kit::fileExists("sequences\\title\\load_table.txt")) {
				err::logMessage("LOAD: error, failed to load title sequence as the load_table is not found at: sequences\\title\\load_table.txt");
				return "";
			}
			return "sequences\\title\\load_table.txt";
		}
		else if (state == "level") {
			string campaign;
			int level;
			registar->get("next_campaign", &campaign);
			registar->get("next_level", &level);

			string path = "campaigns\\" + campaign + "\\" + to_string(level) + "\\load_table.txt";

			if not(os_kit::fileExists(path)) {
				err::logMessage("LOAD: error, failed to load requested parameters for last load state event, expected " + path);
				return "";
			}
			else {
				return path;
			}
		}

		err::logMessage("LOAD: error, failed to load, requested state parameter invalid: " + state);
		return "";
	}

	bool parseLoadTable(string filepath) {

		string line;
		ifstream table(filepath);
		while (getline(table, line)) {
			if (str_kit::isTrivialString(line)) {
				continue;
			}

			


		}
	}

public:
	SystemLoader() {
		debugName = "s_loader";
		//empty system
		cacheOnly = true;
		cachedTarget = 0;


		auto newEnt = new Entity(0);
		auto newComponent = new ComponentPosition(0, 0, -3);
		newEnt->addComponent(newComponent->pullForEntity());
		auto newComponent1 = new ComponentGraphics("default");
		newEnt->addComponent(newComponent1->pullForEntity());
		auto newComponent2 = new ComponentAnimation();
		newEnt->addComponent(newComponent2->pullForEntity());
		cycleEnt[3000] = newEnt;
	}

	void precycle(EntityPool* pool) override {
		//push current cycle of ents
		transferBufferedEntsToGame(pool);



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
			return;
		}

		registar->update("loading", false);

	}

};

#endif