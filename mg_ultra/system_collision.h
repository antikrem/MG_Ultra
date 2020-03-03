#ifndef __SYSTEM_COLLISION__
#define __SYSTEM_COLLISION__

#include "mmap.h"

#include "system.h"

#include "component_position.h"
#include "component_collision.h"
#include "component_damage.h"
#include "component_health.h"

#include "component_particle_on_death.h"

#include "global_collision_interface.h"

#include <iostream>


class SystemCollision : public System {
	//internal map of entity types used to internalEntityLists
	MMap<int, shared_ptr<Entity>> internalEntityLists;

	//vector of CollisionEvents
	vector<CollisionEvent> collisionEvents;

	//standard combat interaction
	//will do nothing if correct components are not found
	void standardCombatHandle(shared_ptr<Entity> source, shared_ptr<Entity> target) {
		auto sourceDamage = source->getComponent<ComponentDamage>();
		auto targetHealth = target->getComponent<ComponentHealth>();
		
		if (sourceDamage && targetHealth) {
			targetHealth->damageHealth(sourceDamage->getDamage());
			sourceDamage->setDamage(0);
			sourceDamage->killEntity();

			auto sourcePod = source->getComponent<ComponentParticleOnDeath>();

			if (sourcePod) {
				sourcePod->setAdjustedPosition(source->getComponent<ComponentPosition>()->getPosition3());
			}
		}
	}


	//handles the collision between two collisions
	void handleCollision(shared_ptr<Entity> source, shared_ptr<Entity> target, const CollisionEvent& collisionEvent);

	//conducts collision comparison between two entity types
	void compareCollisionLists(const CollisionEvent& collisionEvent) {
		for (auto source : internalEntityLists.get(collisionEvent.types[COLLISION_SOURCE])) {

			auto pos = source->getComponent<ComponentPosition>()->getPosition3();
			auto& sCol = source->getComponent<ComponentCollision>()->getSpecification(pos);

			for (auto target : internalEntityLists.get(collisionEvent.types[COLLISION_TARGET])) {

				pos = target->getComponent<ComponentPosition>()->getPosition3();
				auto& tCol = target->getComponent<ComponentCollision>()->getSpecification(pos);

				if (CollisionSpecification::isColliding(sCol, tCol)) {
					handleCollision(source, target, collisionEvent);
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

	void precycle(EntityPool* pool) override {
		if (g_collision::isQueuedColisionEvent()) {
			collisionEvents.push_back(g_collision::getQueuedColisionEvent());
		}

	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto pos = getComponent<ComponentPosition>(components);
		auto col = getComponent<ComponentCollision>(components);

		col->update(pos->getPosition3());

		if (!col->isAddedToSystem()) {
			internalEntityLists.add(ent->getType(), ent);
			col->markAddedToSystem();
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
			if (internalEntityLists.count(i.types[COLLISION_SOURCE])
				&& internalEntityLists.count(i.types[COLLISION_TARGET])) {
				compareCollisionLists(i);
			}
		}

	}
};

#endif