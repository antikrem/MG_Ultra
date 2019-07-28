/*
Handles simple tasks like switching levels,
iterating cycle and counter at game loop speed
*/
#ifndef __SYSTEM_GAME_STATE_CONTROL__
#define __SYSTEM_GAME_STATE_CONTROL__

#include "system.h"

#include "registar.h"

class SystemGameStateControl : public System {
public:
	SystemGameStateControl() {
		debugName = "s_gamestate_control";
		//empty system
		cacheOnly = true;
		cachedTarget = 0;
	}

	void precycle(EntityPool* pool) override {
		//Load state is a specific case, 
		//system game state controller is forced to set cycle and cycle_progress to false
		//then quick exit
		bool load_request = false;
		bool loading = false;
		registar->get("load_request", &load_request);
		registar->get("loading", &loading);

		if (load_request) {
			registar->update("cycle", -1);
			registar->update("cycle_progress", false);
			registar->update("load_request", false);
			registar->update("loading", true);
			return;
		}
		else if (loading) {
			return;
		}

		//check if cycle_progress is true
		bool cycle_progress = false;
		registar->get("cycle_progress", &cycle_progress);

		//if cycle progress true increment cycle
		if (cycle_progress) {
			int cycle = 0;
			registar->get("cycle", &cycle);
			registar->update("cycle", cycle + 1);
		}

	}

};

#endif