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

void g_particles::updateBloomFactor(string particleName, float strength) {
	gParticleMaster->setParticleTypeLightSensitivity(particleName, strength);
}

void g_particles::updateMaxLife(string particleName, float lifeDeviation) {
	gParticleMaster->setParticleMaxLifeDeviation(particleName, lifeDeviation);
}

int g_particles::getParticleType(string particleTypeName) {
	return gParticleMaster->getKeyFromName(particleTypeName);
}