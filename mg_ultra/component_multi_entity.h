/** 
 * A Component that allows an entity to contain multiple other entities
 * These entities also exist in the entity pool
 * Works in conjunction with component_spawner
 */
#ifndef __COMPONENT_MULTI_ENTITY__
#define __COMPONENT_MULTI_ENTITY__

#include <atomic>
#include <shared_mutex>
#include <functional>

#include "component.h"
#include "constants.h"
#include "algorithm_ex.h"

#include "component_position.h"
#include "component_die_with_master.h"
#include "component_offset_master.h"
#include "component_offset_once.h"
#include "component_name.h"

#include "scriptable_class.h"

// Forward declaraction that entities exist
class Entity;

class ComponentMultiEntity : public Component, public ScriptableClass<ComponentMultiEntity> {
private:
	int iterator = 0;
	mutex lock;
	vector<shared_ptr<Entity>> internalEntities;

	// Entities with component name will be cached
	map<string, shared_ptr<Entity>> namedLookup;

	Point3 lastMasterPosition = Point3(0.0f);

	// Static function to use on subents of killed entities
	static void killDieWithMaster(shared_ptr<Entity> subEnt) {
		auto dwm = subEnt->getComponent<ComponentDieWithMaster>();

		if (dwm) {
			dwm->setMasterDead();
		}
	}

public:
	// Adds a shared_ptr to this MultiEntities' internal store
	// Returns false if an entity already exists with the same name
	// and will not add to named cache
	bool addEntity(shared_ptr<Entity> newEnt) {
		unique_lock<mutex> lck(lock);

		auto subPos = newEnt->getComponent<ComponentPosition>();
		auto subOff = newEnt->getComponent<ComponentOffsetMaster>();
		auto subOffOnce = newEnt->getComponent<ComponentOffsetOnce>();
		auto subName = newEnt->getComponent<ComponentName>();

		if (subPos && subOff) {
			subPos->setPosition(
				subPos->getPosition3() + subOff->updatePositionalOffset(lastMasterPosition)
			);
		}
		else if (subPos && subOffOnce) {
			subPos->setPosition(
				subPos->getPosition3() + lastMasterPosition
			);
		}

		internalEntities.push_back(newEnt);

		if (subName) {
			if (namedLookup.count(subName->getName()) > 0) {
				return false;
			}

			namedLookup[subName->getName()] = newEnt;
		}

		return true;
	}

	// Clears any dead entities from this internal store
	void clearDeadEntities() {
		unique_lock<mutex> lck(lock);
		//erase dead entities
		unsigned int count = internalEntities.size();
		erase_sequential_if(internalEntities, [](shared_ptr<Entity> &ent) { return !ent->getFlag(); });
		if (count != internalEntities.size()) {
			erase_associative_if(internalEntities, [](shared_ptr<Entity> &ent) { return !ent->getFlag(); });
		}
	}

	// Updates any sub entities with ComponentOffsetMaster
	void updateOffsetSubs(Point3 masterPosition) {
		unique_lock<mutex> lck(lock);
		lastMasterPosition = masterPosition;
		for (auto i : internalEntities) {
			auto subPos = i->getComponent<ComponentPosition>();
			auto subOff = i->getComponent<ComponentOffsetMaster>();
			if (subPos && subOff) {
				subPos->setPosition(
					subPos->getPosition3() + subOff->updatePositionalOffset(masterPosition)
				);
			}
		}
	}

	//resets access iterator
	void resetIterator() {
		iterator = 0;
	}

	// Gets a shared pointer to the next entity
	// Gets nullptr if there is no next entity
	shared_ptr<Entity> nextEntity() {
		unique_lock<mutex> lck(lock);
		if (iterator < (int)internalEntities.size()) {
			return internalEntities[iterator++];
		}
		else {
			return nullptr;
		}
	}

	// Gets first sub entity by type, returns nullptr if not found
	// Can utilise skip to ignore a number of results
	shared_ptr<Entity> getByType(EntityTypes entityType, int skip = 0) {
		unique_lock<mutex> lck(lock);
		for (auto i : internalEntities) {
			if (i->getType() == entityType) {
				skip--;
				if (skip < 0) {
					return i;
				}
			}
		}

		return nullptr;
	}

	// Gets an entity by name
	shared_ptr<Entity> getByName(string name) {
		unique_lock<mutex> lck(lock);
		if (namedLookup.count(name)) {
			return namedLookup[name];
		}
		else {
			return nullptr;
		}
	}

	// Apply a lambda in the form f : shared_ptr<ent> -> void
	// to all internal entities
	void applyFunction(function<void(shared_ptr<Entity>)> lambda) {
		unique_lock<mutex> lck(lock);
		for (auto i : internalEntities) {
			lambda(i);
		}
	}

	// Kill all sub entities marked to die
	void propogateDeathTag() {
		applyFunction(&ComponentMultiEntity::killDieWithMaster);
	}

	// Kill all children
	void killAllChildren() {
		applyFunction(
			[](shared_ptr<Entity> subEnt) {
				subEnt->killEntity();
			}
		);
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentMultiEntity"].setClass(kaguya::UserdataMetatable<ComponentMultiEntity, Component>()
			.setConstructors<ComponentMultiEntity()>()
			.addFunction("kill_children", &ComponentMultiEntity::killAllChildren)
			.addFunction("reset_iterator", &ComponentMultiEntity::resetIterator)
			.addFunction("get_next", &ComponentMultiEntity::nextEntity)
			.addFunction("get", &ComponentMultiEntity::getByName)
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