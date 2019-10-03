#ifndef __SYSTEM__
#define __SYSTEM__

#include <typeindex>
#include <vector>
#include <cassert>
#include <atomic>

#include "constants.h"
#include "entity.h"
#include "pool.h"
#include "vec_kit.h"

#include "registar.h"

#include "success_callback.h"

/*A system works on entities*/
class System {
protected:
	//A name for debugging
	string debugName = "s_base";

	//if false, end executions
	atomic<bool> killed = false;

	//Target components if one wishes to target those instead of handling ents
	//If types is empty then overwite handleEntity, else overwrite handleComponents
	vector<type_index> types;

	//This is an optional field, if types.size is greater than 1, then any entity will require these types
	//to be used in handleComponentMap
	vector<type_index> requiredTypes;

	//Setting this to a value not is not will force cacheHandle and cacheFail to be used
	bool cacheOnly = false;

	//Also alternativly, this system can be used to interact with a cached entity
	//handleEntity and handleComponentMap will not be used
	int cachedTarget = ETNoType;

	//pointer to global registar
	Registar* registar = nullptr;

	//Each system is given a single su
	//only this su should be used
	SuccessCallback sc;

	//Gets called before a cycle
	virtual void precycle(EntityPool* pool) {

	}

	//Gets called after a cycle
	virtual void postcycle(EntityPool* pool) {

	}

	//To be over written if types is of size 0, system will have to keep buffers if interaction are required
	virtual void handleEntity(shared_ptr<Entity> ent, int id) {
		assert(false);
	}

	//This is an alternative to handleEntity
	//To be over written if types is of size greater than zero
	//Components made availible is of type_index specified in types
	virtual void handleComponentMap(map<type_index, shared_ptr<Component>>& components, int entityType, int id) {
		assert(false);
	}

	//An alternative to both, if cachedTarget != ETNoType (0), this will be called if cache grab is sucessful
	//Otherwise cacheFail will be called, must be overwritten if being used
	virtual void cacheHandle(shared_ptr<Entity> ent) {
		assert(false);
	}

	//On cacheHandle fail, this will be called, not needed to be overwritten
	virtual void cacheFail(EntityPool* pool) {
	}

	//Templated static helper function to get a certain type of component out
	//of a component map
	template <class T>
	static shared_ptr<T> getComponent(map<type_index, shared_ptr<Component>>& components) {
		static_assert(is_base_of<Component, T>::value, "Invalid Component conversion: getComponent");
		return dynamic_pointer_cast<T>(components[typeid(T)]);
	}

	//sets associativity for systems caching
	void setSystemsCachingAssociativity(shared_ptr<Entity> ent) {
		vector<type_index> componentTypes;

		for (auto i : types) {
			if (ent->getComponent(i)) {
				componentTypes.push_back(i);
			}
		}

		ent->setSystemCache(debugName, vec_kit::isSubset(componentTypes, requiredTypes));
	}

	//Not to be overridden, simply an interface for system
	void updateEntity(shared_ptr<Entity> ent, int id) {
		shared_ptr<Entity> entity = ent; //copy to explicitly keep pointer
		if (entity == nullptr)
			return; //exit if ent is null

		//if not types, handle the general way
		if not(types.size()) {
			handleEntity(entity, id);
		}
		//otherwise, handle with type lists
		else {
			//look for system caching
			if (!ent->isSystemCached(debugName)) {
				//if not, evaluate caching
				setSystemsCachingAssociativity(ent);
			}
			
			//if the ent is cached, do the standard
			if (ent->isAnAssociatedSystem(debugName)) {
				handleComponentMap(ent->getMapComponent(), ent->getType(), id);
			}
		}

		//entity shared pointer will pass off the stack, freeing reference count
	}
		
public:
	~System() {
		sc.setCompletion(false);
	}

	//ends the system, stopping all future execution
	void endSystem() {
		killed = true;
		sc.setCompletion(false);
	}

	//called before a precycle, system will only be executed if true
	virtual bool executeSystem() {
		return true;
	}

	//sets global registar
	void setGRegistar(Registar* registar) {
		this->registar = registar;
	}

	//Not to be overridden, the act that will iterate over all of the ents and conduct process for a single cycle
	//The pointer to EntityPool must remain valid, MUST
	void systemCycleProcess(EntityPool* pool) {
		if (killed) {
			return;
		}
		precycle(pool);

		//check if the system is associated with a cached entity
		if (cacheOnly || cachedTarget != 0) {
			shared_ptr<Entity> ptr = pool->getCachedEnt(cachedTarget);
			if (ptr) {
				cacheHandle(ptr);
			}
			else {
				cacheFail(pool);
			}

			if (cacheOnly) {
				postcycle(pool);
				return;
			}
		}

		//else handle appropriately
		int it = pool->begin();
		while (it >= 0) {
			updateEntity(pool->getEnt(it), it);
			it = pool->next(it);
		}
		postcycle(pool);
	}

	string getDebugName() {
		return debugName;
	}
};

#endif