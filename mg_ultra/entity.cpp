#include "entity.h"
#include "component_multi_entity.h"
#include "component_spawn_protection.h"

void Entity::killEntity() {
	
	//check for a multient, as sub ents might has componentDieWithMaster
	auto multiEnt = this->getComponent<ComponentMultiEntity>();
	auto spawnProtection = this->getComponent<ComponentSpawnProtection>();

	//check for spawn protection
	if (spawnProtection) {
		spawnProtection->markDeathOccured();
		//check if this is a good enough time to kill the entity
		if (!spawnProtection->readyToDie()) {
			//If its not ready to die, system_spawn_protection will
			//kill later
			return;
		}
	}
	
	flag = false;

	if (componentCleanUpRequired && multiEnt) {
		multiEnt->propogateDeathTag();
		componentCleanUpRequired = false;
	}
}