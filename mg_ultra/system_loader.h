/*Loading is done asyncronously by the loader
Loader runs on game loop master, and uses the registar to control switching of levels
Loader interfaces with gl handler for mgt loading
Loader also has a list of entities on cycle timer
Loader loads a table from file, The file is in ent_table format

Ent_table:


---ent declaraction

1: spawn condition:
@cycle [int] - spawn on cycle
@counter [int] - spawn on counter
@callback [string] - spawn on an emmitted callback
@immediat - spawn now

2: ent declaration
ent [int] - ent direct
ent [string] - ent table look up



*/
#ifndef __SYSTEM_LOADER__
#define __SYSTEM_LOADER__

#include "system.h"

#include "registar.h"

class SystemLoader : public System {
	map<int, Entity*> cycleEnt;
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


	}

};

#endif