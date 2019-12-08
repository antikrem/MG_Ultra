#ifndef __SYSTEM_COLLISION__
#define __SYSTEM_COLLISION__

#include "mmap.h"

#include "system.h"

#include "component_position.h"
#include "component_collision.h"

#define SOURCE 0
#define TARGET 1

#include <iostream>

struct CollisionEvent {
	EntityTypes types[2];
	//if non-empty, execute script on collision
	string suplementaryScript = "";
};

class SystemCollision : public System {
	//internal map of entity types used to internalEntityLists
	MMap<int, shared_ptr<Entity>> internalEntityLists;

	//vector of CollisionEvents
	vector<CollisionEvent> collisionEvents;

	//handles the collision between two collisions
	void handleCollision(shared_ptr<Entity> source, shared_ptr<Entity> target) {
		cout << "collision" << endl;
	}

	//conducts collision comparison between two entity types
	void comparedCollisionLists(CollisionEvent& collisionEvent) {
		for (auto source : internalEntityLists.get(collisionEvent.types[SOURCE])) {
			for (auto target : internalEntityLists.get(collisionEvent.types[TARGET])) {
				
				auto& sCol = source->getComponent<ComponentCollision>()->getSpecification();
				auto& tCol = target->getComponent<ComponentCollision>()->getSpecification();

				if (CollisionSpecification::isColliding(sCol, tCol)) {
					handleCollision(source, target);
				}

			}
		}
	}
public:
	SystemCollision() {
		debugName = "s_collision";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentPosition, ComponentCollision>()
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto pos = getComponent<ComponentPosition>(components);
		auto col = getComponent<ComponentCollision>(components);

		col->update(pos->getPosition3());

		if (!col->getAddedToSystem()) {
			internalEntityLists.add(ent->getType(), ent);
			col->setAddedToSystem();
		}
	}


	void postcycle(EntityPool* pool) override {
		//clear dead entities
		internalEntityLists.conditionalClear(
			[](shared_ptr<Entity> ent) {
				return !ent->getFlag();
			}
		);

		for (auto& i : collisionEvents) {
			if (internalEntityLists.count(i.types[SOURCE])
				&& internalEntityLists.count(i.types[TARGET])) {

			}
		}

	}
};

#endif