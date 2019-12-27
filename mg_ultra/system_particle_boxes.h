/*handles moving particles to box lists*/
#ifndef __SYSTEM_PARTICLE_BOXES__
#define __SYSTEM_PARTICLE_BOXES__

#include "system.h"

#include "component_particle.h"
#include "component_position.h"

#include "particle_master.h"

class SystemParticleBoxes : public System {

	ParticleMaster* particleMaster = nullptr;
public:


	SystemParticleBoxes() {
		debugName = "s_particle_boxes";
		//empty system
		target = SubPoolTarget();
	}

	void setParticleMaster(ParticleMaster* particleMaster) {
		this->particleMaster = particleMaster;
	}

	void precycle(EntityPool* entityPool) {
		//copy particles
		vector<ParticleSpecification> particles;
		particles.reserve(particleMaster->getParticleCount());
		particleMaster->copyParticles(particles);

		//create temp destination for boxes
		vector<BoxData> boxes;
		boxes.reserve(particles.size());
		particleMaster->evaluateParticleList(particles, boxes);

		particleMaster->copyInBoxes(boxes);
	}
};

#endif