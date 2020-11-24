/*Singleton that handles the sort of things
*/
#ifndef __ECS_MASTER__
#define __ECS_MASTER__

#include "system__header.h"
#include "systems_master.h"
#include "timer.h"
#include "graphics_state.h"
#include "graphics_master.h"
#include "particle_master.h"
#include "audio_master.h"
#include "input.h"
#include "script_master.h"
#include "registar.h"
#include "events.h"

class ECSMaster {
	//Master loop control
	bool executing = true;

	//game is pause
	atomic<bool> paused = false;

	//Master entity pool
	EntityPool* entityPool = nullptr;

	//Vector of systemMasters
	map<string, SystemsMaster*> systemsMasters;

	//global graphics state
	GraphicsMaster* gMaster = nullptr;

	//global master of particles
	ParticleMaster* pMaster = nullptr;

	//global audio master
	AudioMaster* aMaster = nullptr;

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
		-system_profiler
	
	master1: ECS_higher_meta_master :
		-system_multi_ent
		-system_bullet_master
		-system_bullet_spawner
		-system_die_with_master
		-system_spawn_protection

	master2: non-essential gameplay
	    -system_animation
		-system_text
		-system_console
		-system_spawner
		-system_background
		-system_health
		-system_transparency
		-system_weather
		-system_point_light
		-system_colour_modulation
		-system_ambient_light
		-system_fog
		-system_directional_lighting_update
		-system_auto_run

	master3: main gameloop
	    -system_timer
		-system_player
		-system_camera
		-system_movement
		-system_rotation
	    -system_game_state_control

	master4: loading
		-system_loader

	master5: collision
		-system_collision

	master6: audio 
		-system_audio
		-system_music

	master7: particles
		-system_particle_spawner
		-system_particle_boxes
		-system_force_applier
		-system_particle_update
		-system_particle_on_death
		
	master8: autorun
		-system_auto_run
	*/
	void createBasicSystems() {
		
		//ring 0
		auto master = newSystemsMaster("m_meta");
		master->setTimer(10, false);
		master->createSystem<SystemGarbageCollector>(registar);
		master->createSystem<SystemBoundsControl>(registar);
		master->createSystem<SystemProfiler>(registar);

		//ring 1
		master = newSystemsMaster("m_h_meta");
		master->setTimer(100, true);
		master->createSystem<SystemMultiEnt>(registar);
		master->createSystem<SystemBulletMaster>(registar);
		master->createSystem<SystemBulletSpawner>(registar);
		master->createSystem<SystemDieWithMaster>(registar);
		master->createSystem<SystemSpawnProtection>(registar);

		//ring 2
		master = newSystemsMaster("m_graphics2");
		master->setTimer(100, true);
		auto animationSystem = master->createSystem<SystemAnimation>(registar);
		animationSystem->setAnimationMaster(gMaster->getAnimationsMaster());
		master->createSystem<SystemSpawner>(registar);
		master->createSystem<SystemBackground>(registar);
		master->createSystem<SystemDriftable>(registar);
		master->createSystem<SystemHealth>(registar);
		master->createSystem<SystemTransparency>(registar);
		master->createSystem<SystemWeather>(registar);
		master->createSystem<SystemPointLightUpdate>(registar);
		master->createSystem<SystemColourModulation>(registar);
		master->createSystem<SystemAmbientLighting>(registar);
		master->createSystem<SystemFog>(registar);
		master->createSystem<SystemUI>(registar);
		master->createSystem<SystemDirectionalLightUpdate>(registar);

		//ring 3
		master = newSystemsMaster("m_gameplay");
		master->setTimer(100, true);
		master->createSystem<SystemTimer>(registar);
		master->createSystem<SystemPlayer>(registar);
		auto cameraSystem = master->createSystem<SystemCamera>(registar);
		cameraSystem->setGState(gMaster->getGraphicsState());
		master->createSystem<SystemMovement>(registar);
		master->createSystem<SystemRotation>(registar);
		master->createSystem<SystemGameStateControl>(registar);
		master->createSystem<SystemDialogue>(registar);

		//ring 4
		master = newSystemsMaster("m_loader");
		master->setTimer(10, true);
		master->createSystem<SystemLoader>(registar);
		master->createSystem<SystemDialogueLoader>(registar);

		//ring 5
		master = newSystemsMaster("m_collision");
		master->setTimer(100, true);
		master->createSystem<SystemCollision>(registar);

		//ring 6
		master = newSystemsMaster("m_audio");
		master->setTimer(50, false);
		auto audioSystem = master->createSystem<SystemAudio>(registar);
		audioSystem->setAudioMaster(aMaster);
		master->createSystem<SystemMusic>(registar);

		//ring 7
		master = newSystemsMaster("m_particles");
		master->setTimer(100, true);
		auto particleSpawnerSystem = master->createSystem<SystemParticleSpawner>(registar);
		particleSpawnerSystem->setParticleMaster(pMaster);
		auto particleBoxesSystem = master->createSystem<SystemParticleBoxes>(registar);
		particleBoxesSystem->setParticleMaster(pMaster);
		auto particleBoxeseUpdateSystem = master->createSystem<SystemParticleUpdate>(registar);
		particleBoxeseUpdateSystem->setParticleMaster(pMaster);
		auto forceApplierSystem = master->createSystem<SystemForceApplier>(registar);
		forceApplierSystem->setParticleMaster(pMaster);
		auto particleOnDeathSystem = master->createSystem<SystemParticleOnDeath>(registar);
		particleOnDeathSystem->setParticleMaster(pMaster);

		//ring 8
		master = newSystemsMaster("m_auto_run");
		master->setTimer(100, true);
		master->createSystem<SystemAutoRun>(registar);

		//ring 9
		master = newSystemsMaster("m_responsive_frontend");
		master->setTimer(100, false);
		auto textSystem = master->createSystem<SystemText>(registar);
		textSystem->setAnimationMaster(gMaster->getAnimationsMaster());
		master->createSystem<SystemConsole>(registar);
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
		registar = new Registar();
		g_registar::setGlobalRegistar(registar);
		gMaster = new GraphicsMaster(entityPool, registar);
		pMaster = new ParticleMaster(gMaster->getGraphicsState()->getAnimationsMaster());
		gMaster->setParticleMaster(pMaster);
		aMaster = new AudioMaster();
		g_audio::setAudioMaster(aMaster);
		
		inputMaster = new InputMaster(gMaster->getWindow());
		input::setCurrentInputMaster(inputMaster);
		scriptMaster = new ScriptMaster();

		//Create basic system masters
		createBasicSystems();
		cout << print() << endl;

		scriptMaster->createAnonymousEntityHandling();

		//start the script handler
		scriptMaster->beginScriptHandling();

		//start the masters
		startMasters();
		
	}

	/*stops ecs loop*/
	~ECSMaster() {
		scriptMaster->disable();
		g_events::closeEventPipeline();


		for (auto i : systemsMasters) {
			delete i.second;
		}

		delete gMaster;
		delete pMaster;
		delete aMaster;
		delete entityPool;
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
				err::logMessage(
					"EVENT: error loading next state, invalid single state " 
					+ data[0]
				);
				return;
			}

		}
		//3 flag is for switching a a level, 1 is campaign name, 2 is level number
		else if (data.size() == 3) {
			if not(data[0] == "level") {
				err::logMessage(
					"EVENT: error loading next state, invalid triple state " 
					+ data[0] 
					+ ". \n only valid state is \"level\" "
				);
				return;
			}
			
			string campaign = data[1];
			//set second section
			bool valid;
			int level = str_kit::stringToInt(data[2], &valid);
			
			if not(valid) {
				err::logMessage(
					"EVENT: level value is invalid, provided value is: " 
					+ data[2]
				);
				return;
			}

			//values valid, set next_states
			registar->update("next_state", data[0]);
			registar->update("next_campaign", campaign);
			registar->update("next_level", level);

		}
		else {
			err::logMessage(
				"EVENT: error, loading next state, invalid number of state parameters: " 
				+ str_kit::reconstituteVectorIntoString(data, " ")
			);
			return;
		}
		registar->update("load_request", true);
	}

	void handleSystemsInvoke(Event* event) {
		if (event->data.size() != 3) {
			err::logMessage(
				"EVENT: error, expected 3 parameters, got: " 
				+ to_string(event->data.size()) 
				+ " " 
				+ str_kit::reconstituteVectorIntoString(event->data, " ")
			);
			return;
		}

		if (!systemsMasters.count(event->data[0])) {
			err::logMessage(
				"EVENT: error, expected a known system name and [timer/starter], got: " 
				+ event->data[0]
			);
			return;
		}
		
		//ignore paused case
		if not(paused && (event->data[2]) == "p") {
			systemsMasters[event->data[0]]->executeSystemMaster();
		}

		
	}

	void handleEvents() {
		Event* event = nullptr;
		if (!g_events::pollEvents(&event)) {
			return;
		}

		if (g_events::queueSize() > 0
				(g_events::queueSize() % 1000 == 0) 
				&& event->data.size()
			) {
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

		//update the profiling information
		case EV_profilingUpdate:
			updateProfileMap();
			break;
		case EV_pause:
			paused = !paused;
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
			inputMaster->updateKeyPressStates(gMaster->getWindow());

			//handle events
			handleEvents();

			//check executing conditions
			if (glfwWindowShouldClose(gMaster->getWindow())) {
				g_events::pushEvent(new Event(EV_quit));
			}

			// Update global timers
			g_timer::updateTimers();
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

	//sets the profile map in global funcs
	void updateProfileMap() {
		ProfileMap profileInfo;
		for (auto i : systemsMasters) {
			auto info = i.second->getProfileInfo();
			profileInfo[i.first] = {
				get<0>(info),
				get<1>(info),
				get<2>(info)
			};
		}
		setProfileInfoMap(profileInfo);
	}
};

#endif