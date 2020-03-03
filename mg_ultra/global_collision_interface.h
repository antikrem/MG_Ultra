#ifndef __GLOBAL_COLLISION_INTERFACE__
#define __GLOBAL_COLLISION_INTERFACE__

#include "cus_struct2.h"
#include <atomic>

#include "entity_types.h"

#define COLLISION_SOURCE 0
#define COLLISION_TARGET 1

struct CollisionEvent {
	EntityTypes types[2];
	//if non-empty, execute script on collision
	string suplementaryScript = "";

	CollisionEvent(EntityTypes source, EntityTypes target, const string& suplementaryScript = "") {
		types[COLLISION_SOURCE] = source;
		types[COLLISION_TARGET] = target;
		this->suplementaryScript = suplementaryScript;
	}
};

//contains an interface for 
namespace g_collision {

	//Registers a new collision
	void registerNewCollision(int source, int target);

	//Registers a new collision with supplementary script
	void registerNewCollisionWithScript(int source, int target, string script);

	//is true when theres a collision in the queeue
	bool isQueuedColisionEvent();

	//gets first collision event qeueued
	CollisionEvent getQueuedColisionEvent();
};



#endif