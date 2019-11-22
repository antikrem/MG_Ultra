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

class ComponentMultiEntity : public Component, public ScriptableClass {
private:
	int iterator = 0;
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

	//resets access iterator
	void resetIterator() {
		iterator = 0;
	}

	//gets a shared pointer to the next entity
	//gets nullptr if there is no next entity
	shared_ptr<Entity> nextEntity() {
		unique_lock<shared_mutex> lck(lock);
		if (iterator < (int)internalEntities.size()) {
			return internalEntities[iterator++];
		}
		else {
			return nullptr;
		}
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentMultiEntity"].setClass(kaguya::UserdataMetatable<ComponentMultiEntity, Component>()
			.setConstructors<ComponentMultiEntity()>()
			.addFunction("reset_iterator", &ComponentMultiEntity::resetIterator)
			.addFunction("get_next", &ComponentMultiEntity::nextEntity)
			.addStaticFunction("create",ScriptableClass::create<ComponentMultiEntity>)
			.addStaticFunction("type", &getType<ComponentMultiEntity>)
			.addStaticFunction("cast", &Component::castDown<ComponentMultiEntity>)
		);
		//Additionally, add this extra function allowing a function to be 
		//applied to all the entities in a multiend
		state.dostring(
			"function ComponentMultiEntity.for_each(_esComp, _func, ...) \n"
			"	_esComp:reset_iterator() \n"
			"	local iter = _esComp:get_next() \n"
			"	while not is_nil(iter) do \n"
			"		_func(iter, ...) \n"
			"		iter = _esComp:get_next() \n"
			"	end\n"
			"end"
		);
	}
	
};


#endif