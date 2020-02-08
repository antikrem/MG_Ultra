/*handles cleaning up entities that have died
but havn't met spawn death threshold*/
#ifndef __SYSTEM_SPAWN_PROTECTION__
#define __SYSTEM_SPAWN_PROTECTION__

#include "component_spawn_protection.h"

#include "script_master.h"

#include "system.h"

class SystemSpawnProtection : public System {

	EntityPool* entityPool = nullptr;

public:
	SystemSpawnProtection() {
		debugName = "s_spawn_protection";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentSpawnProtection>()
		);
	}

	void precycle(EntityPool* entityPool) override {
		this->entityPool = entityPool;
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto sp = getComponent<ComponentSpawnProtection>(components);

		sp->update();

		if (sp->isMarkedDeathAwaiting()) {
			sp->killEntity();
		}
	}

};

#endif