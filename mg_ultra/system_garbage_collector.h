/*A simple system which calls the clear function 
when it is detected that there are ents to be cleared*/
#ifndef __SYSTEM_GARBAGE_COLLECTOR__
#define __SYSTEM_GARBAGE_COLLECTOR__

#include "system.h"

class SystemGarbageCollector : public System {
private:
	EntityPool* lastPool = nullptr;
	//if true, a entity will need to be cleared
	bool toBeCollected = false;

public:
	SystemGarbageCollector() {
		debugName = "s_garbage_collector";
		target = SubPoolTarget(SubPoolComponents());
	}

	//find ents that need be cleared
	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		ent->entityUpdate();
		if (!ent->getFlag()) {
			toBeCollected = true;
			lastPool->sendToGraveYard(ent);
		}
	}

	//set false for new cycle
	void precycle(EntityPool* pool) override {
		lastPool = pool;
		toBeCollected = false;
	}

	//if an ent needs to be cleared, do so
	void postcycle(EntityPool* pool) override {
		if (toBeCollected) {
			pool->clearDeadEnts();
		}

		//if gravekeep is active, need to clear
		if (pool->getGravekeep() && pool->getGraveyardSize()) {
			pool->clearGraveyard();
		}
	}
};
#endif