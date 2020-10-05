#include "system_collision.h"

#include "script_master.h"

void SystemCollision::handleCollision(shared_ptr<Entity> source, shared_ptr<Entity> target, const CollisionEvent& collisionEvent) {
	standardCombatHandle(source, target);
	standardCombatHandle(target, source);

	deathEmitterHandle(source, target);
	deathEmitterHandle(target, source);

	if (collisionEvent.suplementaryScript.size()) {
		ScriptUnit su(
			SS_functionalCallBack,
			collisionEvent.suplementaryScript
		);
		su.attachEntity(source);
		su.attachEntity(target);
		su.addDebugData("Collision event for " + to_string(source->getType()) + " against " + to_string(target->getType()));

		sc.reset();
		su.attachSuccessCallback(&sc);
		g_script::executeScriptUnit(su);
		sc.waitForCompletion();
	}
}