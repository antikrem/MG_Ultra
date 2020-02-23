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

void g_particles::updateTypeResponse(string particleName, int response) {
	gParticleMaster->setParticleTypeResponse(particleName, (ParticleBoxResponse)response);
}

void g_particles::updateBoxDimension(string particleName, float x, float y, float z) {
	Point3 widths(x, y, z);
	gParticleMaster->setParticleTypeBoxDimension(particleName, widths);
}

void g_particles::updateBoxCenter(string particleName, float x, float y, float z) {
	Point3 position(x, y, z);
	gParticleMaster->setParticleTypeBoxCenter(particleName, position);
}

void g_particles::updateRotateToFace(string particleName, bool rotateToFace) {
	gParticleMaster->setParticleTypeRotateToFace(particleName, rotateToFace);
}

void g_particles::updateFeatherness(string particleName, float featherMean, float featherDeviation) {
	gParticleMaster->setParticleTypeFeatherness(particleName, featherMean, featherDeviation);
}

void g_particles::updateScrollness(string particleName, float scrollness) {
	gParticleMaster->setParticleTypeScrollness(particleName, scrollness);
}

void g_particles::updateWeight(string particleName, float weightMean, float weightDeviation) {
	gParticleMaster->setParticleTypeWeight(particleName, weightMean, weightDeviation);
}


int g_particles::getParticleType(string particleTypeName) {
	return gParticleMaster->getKeyFromName(particleTypeName);
}

ParticleTypeSpecification g_particles::getTypeSpec(int key) {
	return gParticleMaster->getTypeSpecification(key);
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