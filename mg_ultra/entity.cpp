#include "entity.h"
#include "component_multi_entity.h"

void Entity::killEntity() {
	flag = false;
	//check for a multient, as sub ents might has componentDieWithMaster
	auto multiEnt = this->getComponent<ComponentMultiEntity>();

	if (componentCleanUpRequired && multiEnt) {
		multiEnt->propogateDeathTag();
		componentCleanUpRequired = false;
	}
}