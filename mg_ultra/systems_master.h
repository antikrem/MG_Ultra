#ifndef __SYSTEMS_MASTER__
#define __SYSTEMS_MASTER__

#include "system.h"
#include "pool.h"

#include <future>

#define MICROSECONDS_IN_SECOND 1000000

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

	//if true in execution, uses a bool rather than the future to keep track of future, 
	atomic<bool> inExec = false;
	//The next cycle to be executed will be a late execution
	atomic<bool> lateCall = false;
	
	//timer variables
	bool timer = false; //If true, this master will execute async on cycle, otherwise executes when ever touched
	unsigned long long int microsecondsPerCycle = 0; //number of microseconds before next call
	unsigned long long int lastCall = 0; //timer when last called

	//Works the entity pool, applying all the systems
	void workEntityPool() {
		for (auto i : systems) {
			i->systemCycleProcess(entityPool);
		}
	}

	//cycles through the systems, applying what ever needs to be done
	void cycleSystems() {
		for (auto system : systems) {
			if (system->executeSystem()) {
				system->systemCycleProcess(entityPool);
			}
		}
		inExec = false;
	}

public:
	SystemsMaster(EntityPool* entityPool, string name) {
		this->entityPool = entityPool;
		//sets a value in lastFuture, for ease
		lastFuture = async(launch::async, &emptyFunc);
		this->name = name;
	}

	~SystemsMaster() {
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

	//Creates the system, and returns a pointer for further manipulation
	//As an option, feed a pointer, which if not null will be filled with size
	template <class T>
	T* createSystem(Registar* registar) {
		T* system = new T();
		system->setGRegistar(registar);
		addSystem(system);
		return system;
	}

	//Puts this system on a timer, each iteration will compute asyncronously
	//N will be the number of times computed per second, set n=0 to turn off timer
	void setTimer(int n) {
		timer = !!n;
		microsecondsPerCycle = n ? MICROSECONDS_IN_SECOND / n : 0; //no divide by zero
	}

	//Calls cycleSystems asyncronously
	//If the function is already in execution, sets latecall to be true
	void callAsyncCycle(unsigned long long int timer) {
		if not(inExec) {
			inExec = true;
			lateCall = false;
			lastCall = timer;
			calls++;
			lastFuture = async(launch::async, &SystemsMaster::cycleSystems, this);
		} 
		//handle if inExec is true and timer is true
		else if (this->timer) {
			lateCall = true;
		}
	}

	//Behaviour depends on timer
	//If timer is active, update will pass unless $timer - $lastCall > $microsecondsPerCycle and $inExec is false, resulting in call
	//If no timer is active, if not $inExec then a call will be made
	void update(unsigned long long int timer) {
		//No timer case
		if not(this->timer) {
			callAsyncCycle(timer);
		}
		else {
			if (timer - lastCall >= microsecondsPerCycle) {
				callAsyncCycle(timer);
			}
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