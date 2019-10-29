/*A Component that allows an entity to contain multiple other entities
These entities also exist in the entity pool
Works in conjunction with component_spawner*/
#ifndef __COMPONENT_MULTI_ENTITY__
#define __COMPONENT_MULTI_ENTITY__

#include <atomic>
#include <shared_mutex>
#include "component.h"
#include "constants.h"
#include "algorithm_ex.h"

#include "scriptable_class.h"

//forward declaraction that entities exist
class Entity;

class ComponentMultiEntity : public Component {
private:
	shared_mutex lock;
	vector<shared_ptr<Entity>> internalEntities;

public:
	//adds a shared_ptr to this MultiEntities' internal store
	void addEntity(shared_ptr<Entity> newEnt) {
		unique_lock<shared_mutex> lck(lock);
		internalEntities.push_back(newEnt);
	}

	//clears any dead entities from this internal store
	void clearDeadEntities() {
		unique_lock<shared_mutex> lck(lock);
		//erase dead entities
		erase_sequential_if(internalEntities, [](shared_ptr<Entity> &ent) { return !ent->getFlag(); });
	}
	
};


#endif