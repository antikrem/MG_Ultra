/* A container for entities used with entities,
 *Can be iterated over safely
 */
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

#include "algorithm_ex.h"
#include "vec_kit.h"

/* This can be asyncly iterated over,
 * Use begin() to get an id int,
 * Use this id and next() to get next pair
 * After the last pair, return negative id, not guranteed: null ptr
 */
class EntityPool : public ScriptableClass<EntityPool> {
private:
	// Locks cache access
	shared_mutex cacheLock;
	// A seperate cache, int refers to the entity type
	// Entities will be in both cache and list. Both are cleared by clearDeadEnts
	map<int, shared_ptr<Entity>> cache;

	// The EntityPool also keeps a list of subpools
	// which is an optimised access to entities
	map<int, SubPool> subPools;

	// If active, will move dead entities to a staging area
	//and only clear sometimes
	atomic<bool> gravekeep = true;
	// Lock for graveyard
	mutex graveyardLock;
	// Graveyard
	vector<shared_ptr<Entity>> graveyard;
	// Count of number of entities cleared
	atomic<int> passed = 0;

public:
	// Add the empty default pool
	EntityPool() {
		subPools[0] = SubPool(SubPoolTarget(SubPoolComponents()));
	}

	// Adds an entity to the pool, will potentially cache
	shared_ptr<Entity> addEnt(shared_ptr<Entity> ent, bool cacheEnt = false) {
		if (ent->isInPool()) {
			err::logMessage("POOL: Error, entity already in pool, operation aborted");
			return nullptr;
		}
		else {
			ent->markAddToPool();
		}

		if (cacheEnt) {
			unique_lock<shared_mutex> lck(cacheLock);
			if (cache.count(ent->getType())) {
				err::logMessage("POOL: Error, entity of cache type: " + to_string(ent->getType()) + " in engine");
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

	// Add multiple entities in a single cached add
	void addEnts(vector<shared_ptr<Entity>> ents) {
		if (!ents.size()) {
			return;
		}

		set<type_index> signature = ents[0]->getComponentSignature();

		for (unsigned int i = 1; i < ents.size(); i++) {
			if (signature != ents[i]->getComponentSignature()) {
				err::logMessage("POOL: Error, add_entities contained entities with different signatures");
				return;
			}
		}

		for (auto& i : subPools) {
			if (i.second.getTarget().isFullSubPool()
				&& i.second.getTarget().checkEntityInSubPoolTarget(ents[0])) {
				i.second.addEnts(ents);
			}
		}
	}

	shared_ptr<Entity> addEnt(Entity* ent, bool cacheEnt = false) {
		return addEnt(shared_ptr<Entity>(ent), cacheEnt);
	}

	shared_ptr<Entity> l_addEnt(Entity* ent) {
		return addEnt(shared_ptr<Entity>(ent), false);
	}

	vector<shared_ptr<Entity>> l_addEnts(vector<Entity*> ents) {
		vector<shared_ptr<Entity>> processed;
		for (auto i : ents) {
			processed.push_back(shared_ptr<Entity>(i));
		}
		addEnts(processed);
		return processed;
	}

	// Adds a cached entity through lua
	bool addCachedEnt(Entity* ent) {
		return (bool)addEnt(shared_ptr<Entity>(ent), true);
	}

	// Returns a system from cache, returns null if the system is not cached
	shared_ptr<Entity> getCachedEnt(int entityType) {
		// Use null type 
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

	// Kills all entities by a type
	int killByType(int entityType) {
		// Copy in all ents
		int count = 0;
		auto list = subPools[0].getAllEnts();
		for (auto i : list) {
			auto type = i->getType();
			if (entityType == EntityTypes::ETNoType || type == entityType) {
				i->killEntity();
				count++;
			}
		}
		return count;
	}

	// Clear dead entities, needs to be sometimes, locks access to ents, so should only be done sometimes
	// Returns how many ents were killed
	int clearDeadEnts() {
		int cleanedEnts = 0;

		// Need to clear each subpool
		for (auto& i : subPools) {
			i.second.clearDeadEnts();
		}

		{
			// Clear cache
			unique_lock<shared_mutex> glck(cacheLock);
			erase_associative_if(cache, [](auto pair) { return !pair.second->getFlag(); });
		}

		return cleanedEnts;
	}

	// Sends the entity to the graveyard
	void sendToGraveYard(shared_ptr<Entity> ent) {
		unique_lock<mutex> glck(graveyardLock);
		graveyard.push_back(ent);
	}


	// Locks and returns size of entitypool
	tuple<int, int> size() {
		unique_lock<shared_mutex> lck(cacheLock);
		return make_tuple(subPools[0].size(), cache.size());
	}

	// Returns size of graveyard
	int getGraveyardSize() {
		unique_lock<mutex> glck(graveyardLock);
		return graveyard.size();
	}

	// Sets gravekeep
	void setGravekeep(bool gravekeep) {
		this->gravekeep = gravekeep;
	}

	// Gets gravekeep
	bool getGravekeep() {
		return this->gravekeep;
	}

	// Get number of entities passed by the graveyard
	int getGraveyardPassed() {
		return this->passed;
	}

	// Clears all entites in the gravekeep
	// That have no other references
	void clearGraveyard() {
		unique_lock<mutex> glck(graveyardLock);

		for (int i = 0; i < (int)graveyard.size(); i++) {
			if (graveyard[i].use_count() == 1) {
				graveyard[i]->markGCReady();
			}
		}

		int oldSize = graveyard.size();
		erase_sequential_if(graveyard, [](shared_ptr<Entity> &ent) { return ent->getGCReady(); });
		passed += (oldSize - graveyard.size());
	}

	// Adds a new system to the entity pool
	// Returns an index to a subpool to query
	// in future operation
	int allocateToSubPool(SubPoolTarget& subpoolTarget) {
		// Check if target is empty
		if (subpoolTarget.isEmpty()) {
			return -1;
		}

		// Check if a subpool currently exists
		for (auto& i : subPools) {
			if (i.second.getTarget() == subpoolTarget) {
				return i.first;
			}
		}

		// Otherwise create a new subpool
		int newIndex = subPools.rbegin()->first + 1;
		subPools[newIndex] = SubPool(subpoolTarget);
		return newIndex;
	}

	// Gets a shared ptr to entity
	// return nullptr at end index
	// negative key indicates an empty request
	shared_ptr<Entity> getFromSubPool(int key, int index) {
		return key < 0 ? nullptr : subPools[key].getEntity(index);
	}

	static void registerToLua(kaguya::State& state) {
		state["EntityPool"].setClass(
			kaguya::UserdataMetatable<EntityPool>()
			.setConstructors<EntityPool()>()
			.addFunction("addSingular", &EntityPool::l_addEnt)
			.addFunction("addMultiple", &EntityPool::l_addEnts)
			.addFunction("addCached", &EntityPool::addCachedEnt)
			.addFunction("killByType", &EntityPool::killByType)
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