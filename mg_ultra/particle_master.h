//Organises particle managing
#ifndef __PARTICLE_MASTER__
#define __PARTICLE_MASTER__

#include <string>
#include <vector>
#include <map>
#include <shared_mutex>

#include "algorithm_ex.h"

#include "particle.h"
#include "particle_type.h"
#include "force_applier.h"

class ParticleMaster {
	AnimationsMaster* animationMaster = nullptr;

	shared_mutex particleTypeLock;
	int particleTypeAllocator = 0;
	map<int, ParticleType> particleTypes;
	map<string, int> particleKeys;

	mutex particlesLock;
	vector<Particle> particles;

	//most recent list of availible boxes for particles
	mutex boxesLock;
	vector<BoxData> boxes;

	//list of force appliers
	mutex forceSpecificationLock;
	vector<ForceSpecification> forceSpecifications;

	//returns a point3 of momentum from a given force specification
	Point3 computeMommentum(ForceSpecification& forceSpec, Point3 particlePos) { 
		return forceSpec.pos.directionTo(particlePos)
			* (1.0f / (forceSpec.f * pow((particlePos - forceSpec.pos).magnitude(), 2) + forceSpec.p));
	}

public:

	ParticleMaster(AnimationsMaster* animationMaster);

	//returns a key for a given particle name
	//-1 if not found
	int getKeyFromName(string name) {
		shared_lock<shared_mutex> lck(particleTypeLock);
		return particleKeys.count(name) ? particleKeys[name] : -1;
	}

	//Registers a particle type for use
	//returning an int of key for access, -1 on error
	int registerNewParticleType(string particleName, string animationSetName, int animation = 1) {
		unique_lock<shared_mutex> lck(particleTypeLock);
		particleTypes[particleTypeAllocator] = ParticleType(animationSetName, animationMaster, animation);
		particleKeys[particleName] = particleTypeAllocator;

		return particleTypeAllocator++;
	}

	//modifies named particle type's light sensitivity
	void setParticleTypeLightSensitivity(string particleName, float lightSensitivity) {
		if (particleKeys.count(particleName)) {
			particleTypes[particleKeys[particleName]].setBloomFactor(lightSensitivity);
		}
		else {
			err::logMessage("PARTICLE: Was not able to find named particle " + particleName);
		}
	}

	//modifies named particle type's max life deviation
	void setParticleMaxLifeDeviation(string particleName, float maxLifeDeviation) {
		if (particleKeys.count(particleName)) {
			particleTypes[particleKeys[particleName]].maxLifeDeviation = maxLifeDeviation;
		}
		else {
			err::logMessage("PARTICLE: Was not able to find named particle " + particleName);
		}
	}

	//adds a vector of particles
	void addParticles(vector<Particle>& particles) {
		unique_lock<mutex> lck(particlesLock);
		extend(this->particles, particles);
	}

	//gets a count of current number of particles
	int getParticleCount() {
		unique_lock<mutex> lck(particlesLock);
		return particles.size();
	}

	//clears all dead particles in particles
	void clearDeadParticles() {
		unique_lock<mutex> lck(particlesLock);
		erase_sequential_if(
			particles,
			[](Particle& particle) {
				return !particle.active;
			}
		);
	}

	//copies particles for rendering
	void copyParticles(vector<ParticleSpecification>& copy) {
		unique_lock<mutex> lck(particlesLock);
		for (auto& i : particles) {
			copy.push_back(i.getSpecification());
		}
	}

	//evaluates a vector of particles into a vector of boxes
	void evaluateParticleList(vector<ParticleSpecification>& particles, vector<BoxData>& boxes) {
		unique_lock<shared_mutex> lck(particleTypeLock);
		for (auto& i : particles) {
			//check valid key
			if (!particleTypes.count(i.particleKey)) continue;
			ParticleType& particleType = particleTypes[i.particleKey];

			BoxData boxData;
			if (particleType.evaluateBox(i, boxData)) {
				boxes.push_back(boxData);
			}
		}
	}

	//copies over a new list of boxes;
	void copyInBoxes(vector<BoxData>& boxes) {
		unique_lock<mutex> lck(boxesLock);
		this->boxes = boxes;
	}

	//copies into particle master new force specification
	void setForceSpecifications(vector<ForceSpecification>& forceSpecifications) {
		unique_lock<mutex> lck(forceSpecificationLock);
		this->forceSpecifications = forceSpecifications;
	}

	//copies out box data, returns new box bount
	int copyOutBoxes(BoxData* boxes, int start, int size) {
		unique_lock<mutex> lck(boxesLock);
		int boxCount = min(size - start, (int)this->boxes.size());
		memcpy(
			boxes + start, 
			this->boxes.data(), 
			sizeof(BoxData) * max(boxCount, 0)
		);
		return start + boxCount;
	}
	
	//updates particles, uses factoring to split up updating
	void updateParticles(int offset, int factor, const Point3& wind) {
		unique_lock<mutex> lck(particlesLock);
		int size = particles.size();
		for (int i = offset; i < size; i += factor) {
			Point3 mommentum(wind);
			{
				unique_lock<mutex> lck(forceSpecificationLock);
				Point3 pos = particles[i].position;
				for (auto& i : forceSpecifications) {
					mommentum += computeMommentum(i, pos);
				}
			}
			
			particles[i].update(mommentum, (float)factor);
		}
	}

	//gets max life deviation from a particle type
	float getMaxLifeDeviation(int key) {
		if (particleTypes.count(key)) {
			return particleTypes[key].maxLifeDeviation;
		}
		else {
			return 1.0f;
		}
	}
};

namespace g_particles {
	//set particle master
	void setParticleMaster(ParticleMaster* particleMaster);

	//adds a new particle type
	//returns new key
	int addNewParticleType(string particleName, string animationSetName, int animation);

	//updates bloomfactor of named particle
	void updateBloomFactor(string particleName, float strength);

	//update maxLifeDeviation of named particle
	void updateMaxLife(string particleName, float lifeDeviation);

	//returns the key for a given type
	//returns -1 on invalid key
	int getParticleType(string particleTypeName);
}



#endif