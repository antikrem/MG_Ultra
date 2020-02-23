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

		Point3 wind(0.0f);

		registar->get("drift_windspeed_x", &wind.x);
		registar->get("drift_windspeed_y", &wind.y);
		registar->get("drift_windspeed_z", &wind.z);

		Point3 scroll(0.0f);

		registar->get("drift_scrollspeed_x", &scroll.x);
		registar->get("drift_scrollspeed_y", &scroll.y);
		registar->get("drift_scrollspeed_z", &scroll.z);

		particleMaster->updateParticles(trackingOffset, factor, wind, scroll);
		
		particleMaster->clearDeadParticles();
	}
};

#endif