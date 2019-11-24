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

	//target of this entity
	SubPoolTarget target;
	int targetKey;

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

	//This is an alternative to handleEntity
	//To be over written if types is of size greater than zero
	//Components made availible is of type_index specified in types
	virtual void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) {
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
		return components.count(typeid(T)) ? dynamic_pointer_cast<T>(components[typeid(T)]) : nullptr;
	}

	//Not to be overridden, simply an interface for system
	void updateEntity(shared_ptr<Entity> ent, int id) {
		shared_ptr<Entity> entity = ent; //copy to explicitly keep pointer
		if (entity == nullptr) {
			//exit if ent is null
			return; 
		}

		handleComponentMap(ent->getMapComponent(), ent, id);

		//entity shared pointer will pass off the stack, freeing reference count
	}
		
public:
	~System() {
		sc.setCompletion(false);
	}

	//sets the key for this system from pool
	void seedKey(EntityPool* pool) {
		targetKey = pool->allocateToSubPool(target);
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
	//The pointer to EntityPool must remain valid
	void systemCycleProcess(EntityPool* pool) {
		if (killed) {
			return;
		}
		precycle(pool);

		//check if the system is associated with a cached entity
		if (target.isCachedTarget()) {
			shared_ptr<Entity> ptr = pool->getCachedEnt(target.getCachedTarget());
			if (ptr) {
				cacheHandle(ptr);
			}
			else {
				cacheFail(pool);
			}

			if (target.isCacheOnly()) {
				postcycle(pool);
				return;
			}
		}

		//else handle appropriately
		int index = 0;
		auto ent = pool->getFromSubPool(targetKey, index);

		while (ent) {
			updateEntity(ent, index);
			ent = pool->getFromSubPool(targetKey, ++index);
		}

		postcycle(pool);
	}

	string getDebugName() {
		return debugName;
	}
};

#endif