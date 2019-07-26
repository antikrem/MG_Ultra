/*A simple system which calls the clear function 
when it is detected that there are ents to be cleared*/
#ifndef __SYSTEM_GARBAGE_COLLECTOR__
#define __SYSTEM_GARBAGE_COLLECTOR__

#include "system.h"

class SystemGarbageCollector : public System {
private:
	//if true, a entity will need to be cleared
	bool toBeCollected = false;

public:
	SystemGarbageCollector() {
		debugName = "s_garbage_collector";
	}

	//find ents that need be cleared
	void handleEntity(Entity* ent, int id) override {
		ent->entityUpdate();
		toBeCollected = toBeCollected || not( ent->getFlag() );
	}

	//set false for new cycle
	void precycle(EntityPool* pool) override {
		toBeCollected = false;
	}

	//if an ent needs to be cleared, do so
	void postcycle(EntityPool* pool) override {
		if (toBeCollected) {
			pool->clearDeadEnts();
		}
	}
};
#endif