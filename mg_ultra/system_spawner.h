/*A system that adds entity created ents to the pool*/
#ifndef __SYSTEM_SPAWNER__
#define __SYSTEM_SPAWNER__

#include "system.h"
#include "component_spawner.h"
#include "component_multi_entity.h"

class SystemSpawner : public System {
	EntityPool* entityPool = nullptr;

	void precycle(EntityPool* pool) override {
		entityPool = pool;
	}

public:
	SystemSpawner() {
		debugName = "s_spawner";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentSpawner>()
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto spawner = getComponent<ComponentSpawner>(components);

		auto multient = getComponent<ComponentMultiEntity>(components);

		auto dieWithMaster = getComponent<ComponentDieWithMaster>(components);

		vector<Entity*> ents;
		spawner->pullEnts(ents);

		// Algorithm is slightly different depending
		// on if a multi ent exists
		if (!multient) {
			for (auto i : ents) {
				entityPool->addEnt(i, false);
			}
		}

		else {
			for (auto i : ents) {
				shared_ptr<Entity> subEnt = shared_ptr<Entity>(i);
				multient->addEntity(ent, subEnt);
				entityPool->addEnt(subEnt, false);

				// Also add master pointer to die with entity
				if (dieWithMaster) {
					dieWithMaster->setMaster(ent);
				}
			}
		}

	}

};


#endif