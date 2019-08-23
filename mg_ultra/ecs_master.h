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
	vector<SystemsMaster*> systemsMasters;

	//main timer
	Timer* timer = nullptr;

	//global graphics state
	GraphicsState* gState = nullptr;

	//script master
	ScriptMaster* scriptMaster = nullptr;

	//global input master
	InputMaster* inputMaster = nullptr;

	//global registar
	Registar* registar = nullptr;

	SystemsMaster* newSystemsMaster() {
		auto master = new SystemsMaster(entityPool);
		systemsMasters.push_back(master);
		return master;
	}

	/*Creates the most basic system masters
	These masters are:

	master0: ECS_meta_master :
		-system_garbage_collector
		-system_refractor

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
	    -system_game_state_control
		-system_script

	master5: loading
		-system_loader
	
	*/
	void createBasicSystems() {
		//ring 0
		auto master = newSystemsMaster();
		master->createSystem<SystemGarbageCollector>(registar);

		//ring 1
		master = newSystemsMaster();
		auto graphicsMaster = master->createSystem<SystemGraphics>(registar);
		graphicsMaster->setGraphicsState(gState);

		//ring 2
		master = newSystemsMaster();
		auto scriptSystem = master->createSystem<SystemScript>(registar);
		scriptSystem->setScriptMaster(scriptMaster);

		//ring 3
		master = newSystemsMaster();
		master->setTimer(300);
		auto animationSystem = master->createSystem<SystemAnimation>(registar);
		animationSystem->setAnimationMaster(gState->getAnimationsMaster());
		auto textSystem = master->createSystem<SystemText>(registar);
		textSystem->setAnimationMaster(gState->getAnimationsMaster());
		auto consoleSystem = master->createSystem<SystemConsole>(registar);
		auto cameraSystem = master->createSystem<SystemCamera>(registar);
		cameraSystem->setCamera(gState->getCamera());

		//ring 4
		master = newSystemsMaster();
		master->setTimer(300);
		master->createSystem<SystemTimer>(registar);
		master->createSystem<SystemGameStateControl>(registar);


		//ring 5
		master = newSystemsMaster();
		master->setTimer(300);
		master->createSystem<SystemLoader>(registar);

	}

public:

	ECSMaster() {

		entityPool = new EntityPool();
		timer = new Timer();
		gState = new GraphicsState();
		registar = new Registar();
		g_registar::setGlobalRegistar(registar);
		scriptMaster = new ScriptMaster();
		inputMaster = new InputMaster(gState->getWindow());
		input::setCurrentInputMaster(inputMaster);

		registar->addToBase("cycle", -1);
		registar->addToBase("cycle_progress", false);
		registar->addToBase("load_request", false);
		registar->addToBase("loading", false);
		registar->addToBase("state", string("none"));
		registar->addToBase("campaign", string("none"));
		registar->addToBase("level", 0);
		registar->addToBase("next_state", string("none"));
		registar->addToBase("next_campaign", string("none"));
		registar->addToBase("next_level", 0);

		//Create basic system masters
		createBasicSystems();
		cout << print() << endl;

		
		//TESTING
		//add some entities 
		/*
		{
			auto newEnt = new Entity(0);
			auto newComponent = new ComponentPosition(0, 0, -3);
			newEnt->addComponent(newComponent->pullForEntity());
			auto newComponent1 = new ComponentGraphics("default");
			newEnt->addComponent(newComponent1->pullForEntity());
			auto newComponent2 = new ComponentAnimation();
			newEnt->addComponent(newComponent2->pullForEntity());
			entityPool->addEnt(newEnt, true);
		}
		*/
		/*
		//add text ent
		auto newEnt = new Entity(0);
		auto newComponent = new ComponentPosition(0, 0, 0);
		newEnt->addComponent(newComponent->pullForEntity());
		auto newComponent1 = new ComponentText();
		newComponent1->setText("Hello\n World");
		newComponent1->setFont("text_consolas58");
		newEnt->addComponent(newComponent1->pullForEntity());
		entityPool->addEnt(newEnt);
		*/
		
	}

	~ECSMaster() {
		scriptMaster->disable();

		for (auto i : systemsMasters) {
			delete i;
		}

		delete entityPool;
		delete timer;
		delete gState;
		delete inputMaster;
		delete registar;
		delete scriptMaster;
	}

	
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
			err::logMessage("EVENT: error loading next state, invalid number of state parameters: " + str_kit::reconstituteVectorIntoString(data, " "));
			return;
		}
		registar->update("load_request", true);
	}

	void handleEvents() {
		Event event(EV_noEvent);
		while (g_events::pollEvents( &event )) {

			switch (event.type) {

			case EV_noEvent:
				break;

			case EV_quit:
				executing = false;
				break;

			case EV_loadNextState:
				//set next load request flags
				handleLoadRequest(event.data);
				break;
			}

		}
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

			//iterate over system masters, allocating rescources
			for (auto i : systemsMasters) {
				i->update(timer->getTime());
			}

			//check executing conditions
			if (glfwWindowShouldClose( gState->getWindow() )) {
				g_events::pushEvent(Event(EV_quit));
			}
		}
	}

	string print() {
		string message = "MASTERS DEBUG:";
		for (unsigned int i = 0; i < systemsMasters.size(); i++) {
			message += "\n";
			message += systemsMasters[i]->print(i);
		}
		return message;
	}
};

#endif