/*handles cleaning up entities that die due to
reciving damage*/
#ifndef __SYSTEM_BULLET_MASTER__
#define __SYSTEM_BULLET_MASTER__

#include "component_bullet_master.h"
#include "component_multi_entity.h"

#include "script_master.h"

#include "system.h"

class SystemBulletMaster : public System {

	EntityPool* entityPool = nullptr;

	Registar* gRegistar = g_registar::getGlobalRegistar();

public:
	SystemBulletMaster() {
		debugName = "s_bullet_master";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentBulletMaster>()
		);
	}

	void precycle(EntityPool* entityPool) override {
		this->entityPool = entityPool;
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto bm = getComponent<ComponentBulletMaster>(components);
		auto me = getComponent<ComponentMultiEntity>(components);
		
		//if invalid, pass on
		if (!bm->isValid()) {
			return;
		}

		if (!bm->isInitialised()) {
			//need to check this entity has a component_multi_ent
			if (!me) {
				bm->setValid(false);
				return;
			}

			//Need to create the bullet spawner entity
			//and pass to script environment to be set up
			shared_ptr<Entity> ent = shared_ptr<Entity>(new Entity(ETBulletSpawner));

			ScriptUnit su(
				SS_inlineLoader,
				"BulletSpawnerList._initialise(this, \"" + bm->getBulletMasterName() + "\")"
			);
			su.attachEntity(ent);
			su.addDebugData("BulletSpawnerList initialisation for " + bm->getBulletMasterName());

			sc.reset();
			su.attachSuccessCallback(&sc);
			g_script::executeScriptUnit(su);
			bool evaluationSuccess = sc.waitForCompletion();
			int processSuccess;

			//check the script went through and the entity was processed
			if (evaluationSuccess 
				&& gRegistar->get("bullet_spawner_initialisation_success", &processSuccess) 
				&& processSuccess) {
				me->addEntity(ent);
				entityPool->addEnt(ent);
			}
		}

		bm->incrementCycle();
	}

};

#endif