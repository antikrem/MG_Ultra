/*A container for entities used with entities,
Can be iterated over safely*/
#ifndef __POOL__
#define __POOL__

#include <mutex>
#include <map>
#include <shared_mutex>
#include <cassert>
#include "constants.h"
#include "entity.h"

/*This can be asyncly iterated over,
Use begin() to get an id int,
Use this id and next() to get next pair
After the last pair, return negative id, not guranteed: null ptr*/
class EntityPool {
private:
	//Locks during access
	shared_mutex lock;
	//Entities are mapped to ids
	map<int, shared_ptr<Entity>> list;
	//A seperate cache, int refers to the entity type
	//Entities will be in both cache and list. Both are cleared by clearDeadEnts
	map<int, shared_ptr<Entity>> cache;
	//Largest id used
	int largestID = -1;
	//Smallest id within the list
	int smallestID = 0;

	//Updates smallestID, not thread safe, call in the context of lock being aquired uniquely
	void updateSmallestID() {
		for (int i = smallestID; i < largestID; i++) {
			if (list.count(i)) {
				smallestID = i;
				return;
			}

		}
	}

public:
	EntityPool();

	int begin() {
		shared_lock<shared_mutex> lck(lock);
		if not(list.size()) {
			return -1;
		}
		int ret = smallestID;
		return ret;
	}
	
	int next(int id) {
		shared_lock<shared_mutex> lck(lock);
		if (id >= largestID) {
			return -1;
		}
		for (int i = id+1; i <= largestID; id++) {
			if (list.count(i)) {
				return i;
			}
		}
		assert(false);
		return -1;
	}

	//adds a new entity, in a thread safe way
	//set cacheEnt to true to add the cache 
	//returns false if attempted to cache but an ent of the same type already exists
	bool addEnt(Entity* ent, bool cacheEnt = false) {
		unique_lock<shared_mutex> lck(lock);
		largestID++;
		list[largestID] = shared_ptr<Entity>(ent);
		if (cacheEnt) {
			if (cache.count(ent->getType())) {
				return false;
			}
			cache[ent->getType()] = list[largestID];
		}
		return true;
	}

	//alternative method to addEnt to add a shared ptr
	bool addEnt(shared_ptr<Entity> ent, bool cacheEnt = false) {
		unique_lock<shared_mutex> lck(lock);
		largestID++;
		list[largestID] = ent;
		if (cacheEnt) {
			if (cache.count(ent->getType())) {
				return false;
			}
			cache[ent->getType()] = list[largestID];
		}
		return true;
	}

	//On failure, returns null, may fail with a valid id, as that id could have be deleted
	shared_ptr<Entity> getEnt(int id) {
		shared_lock<shared_mutex> lck(lock);
		if (list.count(id)) {
			return list[id];
		}
		else {
			return nullptr;
		}
	}

	//returns a system from cache, returns null if the system is not cached
	shared_ptr<Entity> getCachedEnt(int entityType) {
		//use null type 
		if not(entityType) {
			return nullptr;
		}
		shared_lock<shared_mutex> lck(lock);
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
		//check seperatly, this part is thread safe
		{
			shared_lock<shared_mutex> lck(lock);
			for (auto i : list) {
				i.second->entityUpdate();
			}
		}

		int cleanedEnts = 0;
		unique_lock<shared_mutex> lck(lock);
		
		//clear list
		auto it = list.begin();
		while (it != list.begin()) {
			if (it->second->getFlag()) {
				it++;
			}
			else {
				cleanedEnts++;
				it = list.erase(it);
			}
		}
		
		//clear cache
		it = cache.begin();
		while (it != cache.begin()) {
			if (it->second->getFlag()) {
				it++;
			}
			else {
				it = cache.erase(it);
			}
		}

		updateSmallestID();

		return cleanedEnts;
	}


	//defragments entity position within the pool
	//too many empty slots can cause lag
	void defragment() {
		map<int, shared_ptr<Entity>> copy;
		unique_lock<shared_mutex> lck(lock);
		copy = list;
		list.clear();
		int ptr = -1;
		for (auto i : copy) {
			list[++ptr] = i.second;
		}
		smallestID = 0;
		largestID = ptr;
	}

	//sets all entity pool objects that dont have the saveBetweenState to have death flag
	void stateClear() {
		unique_lock<shared_mutex> lck(lock);

	}
};

EntityPool* getLastPool();

#endif