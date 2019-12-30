/*updates all particles*/
#ifndef __SYSTEM_PARTICLE_UPDATE__
#define __SYSTEM_PARTICLE_UPDATE__

#include "system.h"

#include "component_particle.h"
#include "component_position.h"

#include "particle_master.h"

class SystemParticleUpdate : public System {
	ParticleMaster* particleMaster = nullptr;

	int trackingOffset = 0;
	int factor = 1;
public:


	SystemParticleUpdate() {
		debugName = "s_particle_update";
		//empty system
		target = SubPoolTarget();
	}

	void setParticleMaster(ParticleMaster* particleMaster) {
		this->particleMaster = particleMaster;
	}

	void precycle(EntityPool* entityPool) {
		trackingOffset = (trackingOffset + 1) % factor;
		particleMaster->updateParticles(trackingOffset, factor);
		particleMaster->clearDeadParticles();
	}
};

#endif