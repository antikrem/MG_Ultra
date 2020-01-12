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

void  g_particles::updateTypeResponse(string particleName, int response) {
	gParticleMaster->setParticleTypeResponse(particleName, (ParticleBoxResponse)response);
}

void  g_particles::updateBoxDimension(string particleName, float x, float y, float z) {
	Point3 widths(x, y, z);
	gParticleMaster->setParticleTypeBoxDimension(particleName, widths);
}

void  g_particles::updateBoxCenter(string particleName, float x, float y, float z) {
	Point3 position(x, y, z);
	gParticleMaster->setParticleTypeBoxCenter(particleName, position);
}

int g_particles::getParticleType(string particleTypeName) {
	return gParticleMaster->getKeyFromName(particleTypeName);
}

int g_particles::clearParticles() {
	return gParticleMaster->clearParticles();
}

Point3 g_particles::getBoundingBoxSize(int key) {
	return gParticleMaster->getBoundingBoxSize(key);
}

Point3 g_particles::getBoundingBoxCenter(int key) {
	return gParticleMaster->getBoundingBoxCenter(key);

}