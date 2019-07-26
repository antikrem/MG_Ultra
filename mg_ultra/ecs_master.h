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

	master2: animation
	    -system_animation
		-system_text
		-system_console

	master3: main gameloop
		-system_script


	
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
		master->setTimer(300);
		auto animationSystem = master->createSystem<SystemAnimation>(registar);
		animationSystem->setAnimationMaster(gState->getAnimationsMaster());
		auto textSystem = master->createSystem<SystemText>(registar);
		textSystem->setAnimationMaster(gState->getAnimationsMaster());
		auto consoleSystem = master->createSystem<SystemConsole>(registar);

		//ring 3
		master = newSystemsMaster();
		master->setTimer(300);
		master->createSystem<SystemGameStateControl>(registar);
		auto scriptSystem = master->createSystem<SystemScript>(registar);
		scriptSystem->setScriptMaster(scriptMaster);

		//ring 4
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
		registar->addToBase("next_state", string("title"));
		registar->addToBase("next_campaign", string("none"));
		registar->addToBase("next_level", 0);

		//Create basic system masters
		createBasicSystems();
		cout << print() << endl;

		
		//TESTING
		//add some entities 
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

	void handleEvents() {
		Event event(EV_noEvent);
		while (g_events::pollEvents( &event )) {
			switch (event.type) {
			case EV_noEvent:
				return;
			case EV_quit:
				executing = false;
				break;
			}
		}
	}

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