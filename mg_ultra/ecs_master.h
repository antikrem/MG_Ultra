/*Singleton that handles the sort of things
*/
#ifndef __ECS_MASTER__
#define __ECS_MASTER__

#include "system__header.h"
#include "systems_master.h"
#include "timer.h"
#include "graphics_state.h"
#include "input.h"
#include "script_master.h"
#include "registar.h"
#include "events.h"

class ECSMaster {
	//Master loop control
	bool executing = true;

	//Master entity pool
	EntityPool* entityPool = nullptr;

	//Vector of systemMasters
	map<string, SystemsMaster*> systemsMasters;

	//global graphics state
	GraphicsState* gState = nullptr;

	//script master
	ScriptMaster* scriptMaster = nullptr;

	//global input master
	InputMaster* inputMaster = nullptr;

	//global registar
	Registar* registar = nullptr;

	SystemsMaster* newSystemsMaster(string name) {
		auto master = new SystemsMaster(entityPool, name);
		systemsMasters[name] = master;
		return master;
	}

	/*Creates the most basic system masters
	These masters are:

	master0: ECS_meta_master :
		-system_garbage_collector
		-system_bounds_control

	master1: graphics
	    -system_graphics

	master2: scripts
		-system_script

	master3: non-essentaion gameplay
	    -system_animation
		-system_text
		-system_console

	master4: main gameloop
	    -system_timer
		-system_player
		-system_camera
		-system_movement
		-system_rotation
	    -system_game_state_control

	master5: loading
		-system_loader
	
	*/
	void createBasicSystems() {
		
		//ring 0
		auto master = newSystemsMaster("m_meta");
		master->setTimer(250);
		master->createSystem<SystemGarbageCollector>(registar);
		master->createSystem<SystemBoundsControl>(registar);
		
		//ring 1
		master = newSystemsMaster("m_graphics");
		auto graphicsMaster = master->createSystem<SystemGraphics>(registar);
		graphicsMaster->setGraphicsState(gState);

		//ring 2
		master = newSystemsMaster("m_script");
		auto scriptSystem = master->createSystem<SystemScript>(registar);
		scriptSystem->setScriptMaster(scriptMaster);

		//ring 3
		master = newSystemsMaster("m_graphics2");
		master->setTimer(250);
		auto animationSystem = master->createSystem<SystemAnimation>(registar);
		animationSystem->setAnimationMaster(gState->getAnimationsMaster());
		auto textSystem = master->createSystem<SystemText>(registar);
		textSystem->setAnimationMaster(gState->getAnimationsMaster());
		master->createSystem<SystemConsole>(registar);
		master->createSystem<SystemSpawner>(registar);

		//ring 4
		master = newSystemsMaster("m_gameplay");
		master->setTimer(250);
		master->createSystem<SystemTimer>(registar);
		master->createSystem<SystemPlayer>(registar);
		auto cameraSystem = master->createSystem<SystemCamera>(registar);
		cameraSystem->setCamera(gState->getCamera());
		master->createSystem<SystemMovement>(registar);
		master->createSystem<SystemRotation>(registar);
		master->createSystem<SystemGameStateControl>(registar);

		//ring 5
		master = newSystemsMaster("m_loader");
		master->setTimer(300);
		master->createSystem<SystemLoader>(registar);
	}

	//starts all of the masters
	void startMasters() {
		for (auto i : systemsMasters) {
			i.second->start();
		}
	}

public:

	ECSMaster() {

		entityPool = new EntityPool();
		setGlobalPool(entityPool);
		gState = new GraphicsState();
		registar = new Registar();
		g_registar::setGlobalRegistar(registar);
		inputMaster = new InputMaster(gState->getWindow());
		input::setCurrentInputMaster(inputMaster);
		scriptMaster = new ScriptMaster();

		

		//Create basic system masters
		createBasicSystems();
		cout << print() << endl;

		//start the masters
		startMasters();
		
	}

	~ECSMaster() {
		scriptMaster->disable();
		g_events::closeEventPipeline();


		for (auto i : systemsMasters) {
			delete i.second;
		}

		delete entityPool;
		delete gState;
		delete inputMaster;
		delete registar;
		delete scriptMaster;
	}

	/*stops ecs loop*/

	
private:

	void handleLoadRequest(vector<string>& data) {
		
		//single flag is for switching to simple states like title
		if (data.size() == 1) {
			if (data[0] == "title") {
				registar->update("next_state", data[0]);
			}
			else {
				err::logMessage("EVENT: error loading next state, invalid single state " + data[0]);
				return;
			}

		}
		//3 flag is for switching a a level, 1 is campaign name, 2 is level number
		else if (data.size() == 3) {
			if not(data[0] == "level") {
				err::logMessage("EVENT: error loading next state, invalid triple state " + data[0] + ". \n only valid state is \"level\" ");
				return;
			}
			
			string campaign = data[1];
			//set second section
			bool valid;
			int level = str_kit::stringToInt(data[2], &valid);
			
			if not(valid) {
				err::logMessage("EVENT: level value is invalid, provided value is: " + data[2]);
				return;
			}

			//values valid, set next_states
			registar->update("next_state", data[0]);
			registar->update("next_campaign", campaign);
			registar->update("next_level", level);

		}
		else {
			err::logMessage("EVENT: error, loading next state, invalid number of state parameters: " + str_kit::reconstituteVectorIntoString(data, " "));
			return;
		}
		registar->update("load_request", true);
	}

	void handleSystemsInvoke(Event* event) {
		if (event->data.size() != 2) {
			err::logMessage("EVENT: error, expected 2 parameters, got: " + to_string(event->data.size()) + " " + str_kit::reconstituteVectorIntoString(event->data, " "));
			return;
		}

		if (!systemsMasters.count(event->data[0])) {
			err::logMessage("EVENT: error, expected a known system name and [timer/starter], got: " + event->data[0]);
			return;
		}
		
		systemsMasters[event->data[0]]->executeSystemMaster();
	}

	void handleEvents() {
		Event* event = nullptr;
		g_events::pollEvents(&event);

		if (g_events::queueSize() > 1000 && event->data.size()) {
			cout << "cycleing " << event->data[0] << " " << g_events::queueSize() << endl;
		}
		

		switch (event->type) {
		//no event, no effect
		case EV_noEvent:
			break;

		//quit game
		case EV_quit:
			executing = false;
			break;

		//loads the next game state
		case EV_loadNextState:
			//set next load request flags
			handleLoadRequest(event->data);
			break;
		
		//invokes a systems master's system cycle
		case EV_invokeSystemMaster:
			//invoke a system master
			handleSystemsInvoke(event);
			break;
		}

		delete event;
	}

public:

	void gameloop() {
		while (executing) {
			//poll glfw events
			glfwPollEvents();

			//update input master
			inputMaster->updateKeyPressStates(gState->getWindow());

			//handle events
			handleEvents();

			//check executing conditions
			if (glfwWindowShouldClose( gState->getWindow() )) {
				g_events::pushEvent(new Event(EV_quit));
			}
		}
	}

	string print() {
		string message = "MASTERS DEBUG:";
		int i = 0;
		for (auto it = systemsMasters.begin(); it != systemsMasters.end(); it++) {
			message += "\n";
			message += it->second->print(i);
			i++;
		}
		return message;
	}
};

#endif