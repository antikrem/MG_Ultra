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


/*A system works on entities*/
class System {
protected:
	//A name for debugging
	string debugName = "s_base";

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
	virtual void handleComponentMap(map<type_index, Component*> components, int entityType, int id) {
		assert(false);
	}

	//An alternative to both, if cachedTarget != ETNoType (0), this will be called if cache grab is sucessful
	//Otherwise cacheFail will be called, must be overwritten if being used
	virtual void cacheHandle(Entity* ent) {
		assert(false);
	}

	//On cacheHandle fail, this will be called, not needed to be overwritten
	virtual void cacheFail(EntityPool* pool) {
	}


	//keeps a reference to the shared entities
	//not to be over written
	void createComponentList(Entity* ent, int id) {
		vector<type_index> componentTypes;
		vector<shared_ptr<Component>> shareds;
		map<type_index, Component*> components;
		shared_ptr<Component> comp = nullptr;

		for (auto i : types) {
			if (comp = ent->getComponent(i)) {
				shareds.push_back(comp);
				componentTypes.push_back(i);
				components[i] = comp.get();
			}
		}

		if (vec_kit::isSubset(componentTypes, requiredTypes)) {
			handleComponentMap(components, ent->getType(), id);
		}
		//shareds passes, freeing shared pointers
	}

	//Not to be overridden, simply an interface for system
	void updateEntity(shared_ptr<Entity> ent, int id) {
		shared_ptr<Entity> entity = ent; //copy to explicitly keep pointer
		if (entity == nullptr)
			return; //exit if ent is null

		if not(types.size())
			handleEntity(entity, id);
		else
			createComponentList(entity.get(), id);
		//entity shared pointer will pass off the stack, freeing reference count
	}
		
public:
	//sets global registar
	void setGRegistar(Registar* registar) {
		this->registar = registar;
	}

	//Not to be overridden, the act that will iterate over all of the ents and conduct process for a single cycle
	//The pointer to EntityPool must remain valid, MUST
	void systemCycleProcess(EntityPool* pool) {
		precycle(pool);

		//check if the system is cached first
		if (cachedTarget != 0 || cacheOnly) {
			shared_ptr<Entity> ptr = pool->getCachedEnt(cachedTarget);
			if (ptr) {
				cacheHandle(ptr.get());
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