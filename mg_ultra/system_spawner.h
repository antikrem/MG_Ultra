/*A system that adds entity created ents to the pool*/
#ifndef __SYSTEM_SPAWNER__
#define __SYSTEM_SPAWNER__

#include "system.h"
#include "component_spawner.h"

class SystemSpawner : public System {
	EntityPool* entityPool = nullptr;

	void precycle(EntityPool* pool) override {
		entityPool = pool;
	}

public:
	SystemSpawner() {
		types.push_back(typeid(ComponentSpawner));
		requiredTypes.push_back(typeid(ComponentSpawner));
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, int entityType, int id) override {
		auto spawner = getComponent<ComponentSpawner>(components);
		vector<Entity*> ents;
		spawner->pullEnts(ents);
		for (auto i : ents) {
			entityPool->addEnt(i, false);
		}

	}

};


#endif