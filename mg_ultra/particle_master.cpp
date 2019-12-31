#include "particle_master.h"

ParticleMaster* gParticleMaster = nullptr;

void g_particles::setParticleMaster(ParticleMaster* particleMaster) {
	gParticleMaster = particleMaster;
}

ParticleMaster::ParticleMaster(AnimationsMaster* animationMaster) {
	this->animationMaster = animationMaster;
	g_particles::setParticleMaster(this);
}

int g_particles::addNewParticleType(string particleName, string animationSetName, int animation) {
	return gParticleMaster->registerNewParticleType(particleName, animationSetName, animation);
}

int g_particles::getParticleType(string particleTypeName) {
	return gParticleMaster->getKeyFromName(particleTypeName);
}