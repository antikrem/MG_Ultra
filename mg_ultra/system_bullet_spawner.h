/*handles managing bullet spawners*/
#ifndef __SYSTEM_BULLET_SPAWNER__
#define __SYSTEM_BULLET_SPAWNER__

#include "component_bullet_spawner.h"

#include "script_master.h"

#include "system.h"

class SystemBulletSpawner : public System {
public:
	SystemBulletSpawner() {
		debugName = "s_bullet_spawner";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentBulletSpawner>()
		);
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto bs = getComponent<ComponentBulletSpawner>(components);

		bs->incrementCycle();

		ScriptUnit su(
			SS_inlineLoader,
			"BulletSpawnerList._spawnerUpdate(\"" 
			+ bs->getBulletMasterName() + "\", "
			+ to_string(bs->getCycle()) + ", "
			+ to_string(bs->getLastActiceScript()) + ")"
		);

		su.attachEntity(ent);
		
		sc.reset();
		su.attachSuccessCallback(&sc);
		g_script::executeScriptUnit(su);
		sc.waitForCompletion();
	}

};

#endif