#ifndef __SYSTEMS_MASTER__
#define __SYSTEMS_MASTER__

#include "system.h"
#include "pool.h"
#include "timed_event_callback.h"

#include <future>

//A static function that does nothing!
void emptyFunc() {

}

//A object that will handle a bunch of systems, 
//Will do thread behaviour
class SystemsMaster {
	//name of system
	string name;

	//Internal counter of times called
	unsigned long int calls = 0;

	//vector of systems associated with this systems master
	vector<System*> systems;

	//Pointer to main entity pool
	EntityPool* entityPool = NULL;

	//stores the last future
	future<void> lastFuture;

	//if true, this cycle is still on timer, and needs to reexecute 
	atomic<bool> inUse = false;
	//if true in execution, uses a bool rather than the future to keep track of future, 
	atomic<bool> inExec = false;
	//The next cycle to be executed will be a late execution
	atomic<bool> lateCall = false;

	//if not null, used for executing system
	TimedEventCallback* timer = nullptr;

	//DEBUG system currently executed
	System* sys = nullptr;

	//cycles through the systems, applying what ever needs to be done
	void cycleSystems() {
		for (auto system : systems) {
			if (system->executeSystem()) {
				sys = system;
				system->systemCycleProcess(entityPool);
			}
		}

		inExec = false;
		//if not timer, this cycle system will invoke another cycle
		if (!timer && inUse) {
			Event* starterEvent = new Event(EV_invokeSystemMaster);
			starterEvent->data.push_back(name);
			starterEvent->data.push_back("starter");
			g_events::pushEvent(starterEvent);
		}
	}

public:
	SystemsMaster(EntityPool* entityPool, string name) {
		this->entityPool = entityPool;
		//sets a value in lastFuture, for ease
		lastFuture = async(launch::async, &emptyFunc);
		this->name = name;
	}

	~SystemsMaster() {
		inUse = false;
		//if has a timer, stop it
		if (timer) {
			timer->stopTimer();
		}

		//dirty kill each systems sucess callback
		for (auto i : systems) {
			i->endSystem();
		}

		//Spin lock waiting for systems to end
		while (inExec) {
			cout << "spin locked " + name << endl;
		}

		for (auto i : systems) {
			delete i;
		}
	}

	//Adds a system, returns the system count
	int addSystem(System* system) {
		systems.push_back(system);
		return systems.size();
	}

	//Puts this system on a timer, each iteration will compute asyncronously
	//N will be the number of times computed per second, set n=0 to turn off timer
	//n=0 causes finite recursion, where the end of each cycle causes the next one 
	void setTimer(int n) {
		timer = new TimedEventCallback(n, name);
	}

	//starts the systems execution
	void start() {
		inUse = true;
		if (timer) {
			//start the timer to send off events
			timer->startTimer();
		} 
		else {
			//else, send off the starting event
			Event* starterEvent = new Event(EV_invokeSystemMaster);
			starterEvent->data.push_back(name);
			starterEvent->data.push_back("starter");
			g_events::pushEvent(starterEvent);
		}
	}

	//Creates the system, and returns a pointer for further manipulation
	//As an option, feed a pointer, which if not null will be filled with size
	template <class T>
	T* createSystem(Registar* registar) {
		T* system = new T();
		system->setGRegistar(registar);
		addSystem(system);
		return system;
	}

	//Calls cycleSystems asyncronously
	//If the function is already in execution, sets latecall to be true
	void executeSystemMaster() {
		if not(inExec) {
			inExec = true;
			lateCall = false;
			calls++;
			lastFuture = async(launch::async, &SystemsMaster::cycleSystems, this);
		} 
		//handle if inExec is true
		else {
			lateCall = true;
		}
	}

	//print a debug message
	string print() {
		string message = "";
		message += ("BOUNDED SYSTEMS:");
		for (auto i : systems) {
			message += "\n |-";
			message += i->getDebugName();
		}
		return message;
	}

	//prints a debug message with the level formatted
	string print(int level) {
		vector<string> debugs;
		for (auto i : systems) {
			debugs.push_back(i->getDebugName());
		}
		return ("MASTER @ LEVEL " + to_string(level) + '\n') + str_kit::createBranchFromVector(debugs);
	}
};

#endif