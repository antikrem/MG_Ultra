/*updates the force applier list in particle master*/
#ifndef __SYSTEM_FORCE_APPLIER__
#define __SYSTEM_FORCE_APPLIER__

#include <mutex>
#include <vector>

#include "system.h"

#include "component_force_applier.h"
#include "component_position.h"

#include "particle_master.h"

class SystemForceApplier : public System {

	ParticleMaster* particleMaster = nullptr;

	vector<ForceSpecification> forceSpecificationCache;
public:

	SystemForceApplier() {
		debugName = "s_force_applier";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentForceApplier>()
		);
	}

	void setParticleMaster(ParticleMaster* particleMaster) {
		this->particleMaster = particleMaster;
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		shared_ptr<ComponentForceApplier> comFor = getComponent<ComponentForceApplier>(components);
		shared_ptr<ComponentPosition> comPos = getComponent<ComponentPosition>(components);

		//check if comFor is initialised
		if (comPos) {
			comFor->setPosition(comPos->getPosition3());
			forceSpecificationCache.push_back(comFor->getForceApplier());
		}
	}

	void postcycle(EntityPool* entityPool) {
		particleMaster->setForceSpecifications(forceSpecificationCache);
		forceSpecificationCache.clear();
	}
};


#endif