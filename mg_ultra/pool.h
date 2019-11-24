/*A container for entities used with entities,
Can be iterated over safely*/
#ifndef __POOL__
#define __POOL__

#include <mutex>
#include <vector>
#include <map>
#include <shared_mutex>
#include <algorithm>

#include "constants.h"
#include "entity.h"
#include "subpooling.h"

#include "scriptable_class.h"

#include "algorithm_ex.h"

/*This can be asyncly iterated over,
Use begin() to get an id int,
Use this id and next() to get next pair
After the last pair, return negative id, not guranteed: null ptr*/
class EntityPool : public ScriptableClass {
private:
	//Locks cache access
	shared_mutex cacheLock;
	//A seperate cache, int refers to the entity type
	//Entities will be in both cache and list. Both are cleared by clearDeadEnts
	map<int, shared_ptr<Entity>> cache;

	//The EntityPool also keeps a list of subpools
	//which is an optimised access to entities
	map<int, SubPool> subPools;

	//if active, will move dead entities to a staging area
	//and only clear sometimes
	atomic<bool> gravekeep = true;
	//lock for graveyard
	mutex graveyardLock;
	//graveyard
	vector<shared_ptr<Entity>> graveyard;
	//count of number of entities cleared
	atomic<int> passed = 0;

public:
	//Add the empty default pool
	EntityPool() {
		subPools[0] = SubPool(SubPoolTarget(SubPoolComponents()));
	}

	//adds an entity to the pool, will potentially cache
	shared_ptr<Entity> addEnt(shared_ptr<Entity> ent, bool cacheEnt = false) {
		if (cacheEnt) {
			unique_lock<shared_mutex> lck(cacheLock);
			if (cache.count(ent->getType())) {
				return nullptr;
			}
			cache[ent->getType()] = ent;
		}

		for (auto& i : subPools) {
			if (i.second.getTarget().isFullSubPool() 
				&& i.second.getTarget().checkEntityInSubPoolTarget(ent)) {
					i.second.addEnt(ent);
			}
		}
		return ent;
	}

	shared_ptr<Entity> addEnt(Entity* ent, bool cacheEnt = false) {
		return addEnt(shared_ptr<Entity>(ent), cacheEnt);
	}

	//returns a system from cache, returns null if the system is not cached
	shared_ptr<Entity> getCachedEnt(int entityType) {
		//use null type 
		if (entityType == ETNoType) {
			return nullptr;
		}
		shared_lock<shared_mutex> lck(cacheLock);
		if (cache.count(entityType)) {
			return cache[entityType];
		}
		else {
			return nullptr;
		}
	}

	//clear dead entities, needs to be sometimes, locks access to ents, so should only be done sometimes
	//returns how many ents were killed
	int clearDeadEnts() {
		int cleanedEnts = 0;
		
		//update all entities and copy to graveyard
		int key = 0;
		auto ent = subPools[0].getEntity(key);
		while (ent) {
			ent->entityUpdate();
			if (!ent->getFlag()) {
				unique_lock<mutex> glck(graveyardLock);
				graveyard.push_back(ent);
			}
			ent = subPools[0].getEntity(++key);
		}

		{
			unique_lock<shared_mutex> glck(cacheLock);
			erase_associative_if(cache, [](auto pair) { return !pair.second->getFlag(); });
		}

		//need to clear each subpool

		return cleanedEnts;
	}

	//locks and returns size of entitypool
	tuple<int, int> size() {
		unique_lock<shared_mutex> lck(cacheLock);
		return make_tuple(subPools[0].size(), cache.size());
	}

	//returns size of graveyard
	int getGraveyardSize() {
		unique_lock<mutex> glck(graveyardLock);
		return graveyard.size();
	}

	//sets gravekeep
	void setGravekeep(bool gravekeep) {
		this->gravekeep = gravekeep;
	}

	//gets gravekeep
	bool getGravekeep() {
		return this->gravekeep;
	}

	//get number of entities passed by the graveyard
	int getGraveyardPassed() {
		return this->passed;
	}

	//clears all entites in the gravekeep
	//that have no other references
	void clearGraveyard() {
		unique_lock<mutex> glck(graveyardLock);
		int oldSize = graveyard.size();
		erase_sequential_if(graveyard, [](shared_ptr<Entity> &ent) { return ent.use_count() == 1; });
		passed += (oldSize - graveyard.size());
	}

	//Adds a new system to the entity pool
	//returns an index to a subpool to query
	//in future operation
	int allocateToSubPool(SubPoolTarget& subpoolTarget) {
		//check if target is empty
		if (subpoolTarget.isEmpty()) {
			return -1;
		}

		//check if a subpool currently exists
		for (auto& i : subPools) {
			if (i.second.getTarget() == subpoolTarget) {
				return i.first;
			}
		}

		//otherwise create a new subpool
		int newIndex = subPools.rbegin()->first + 1;
		subPools[newIndex] = SubPool(subpoolTarget);
		return newIndex;
	}

	//gets a shared ptr to entity
	//return nullptr at end index
	//negative key indicates an empty request
	shared_ptr<Entity> getFromSubPool(int key, int index) {
		return key < 0 ? nullptr : subPools[key].getEntity(index);
	}

	void registerToLua(kaguya::State& state) override {
		state["EntityPool"].setClass(
			kaguya::UserdataMetatable<EntityPool>()
			.setConstructors<EntityPool()>()
			.addFunction("getEntFromCache", &EntityPool::getCachedEnt)
			.addFunction("getGraveyardSize", &EntityPool::getGraveyardSize)
			.addFunction("getGraveyardPassed", &EntityPool::getGraveyardPassed)
			.addFunction("size", &EntityPool::size)
		);
	}

};

EntityPool* getGlobalPool();

void setGlobalPool(EntityPool* pool);

#endif