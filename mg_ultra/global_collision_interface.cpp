#include "global_collision_interface.h"

#include <mutex>
#include <queue>

mutex collisionLock;
queue<CollisionEvent> collisionQueue;

void g_collision::registerNewCollision(int source, int target) {
	unique_lock<mutex> lck(collisionLock);
	collisionQueue.push(
		CollisionEvent((EntityTypes)source, (EntityTypes)target)
	);
}

void g_collision::registerNewCollisionWithScript(int source, int target, string script) {
	unique_lock<mutex> lck(collisionLock);
	collisionQueue.push(
		CollisionEvent((EntityTypes)source, (EntityTypes)target, script)
	);
}

bool g_collision::isQueuedColisionEvent() {
	unique_lock<mutex> lck(collisionLock);
	return collisionQueue.size();
}

CollisionEvent g_collision::getQueuedColisionEvent() {
	unique_lock<mutex> lck(collisionLock);
	CollisionEvent start = collisionQueue.front();
	collisionQueue.pop();
	return start;
}
