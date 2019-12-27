//Organises particle managing
#ifndef __PARTICLE_MASTER__
#define __PARTICLE_MASTER__

#include <string>
#include <vector>
#include <map>
#include "algorithm_ex.h"

#include "particle.h"
#include "particle_type.h"


class ParticleMaster {
	AnimationsMaster* animationMaster = nullptr;

	mutex particleTypeLock;
	int particleTypeAllocator = 0;
	map<int, ParticleType> particleTypes;
	map<string, int> particleKeys;

	mutex particlesLock;
	vector<Particle> particles;

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

	//most recent list of availible boxes for particles
	mutex boxesLock;
	vector<BoxData> boxes;

public:

	ParticleMaster(AnimationsMaster* animationMaster) {
		this->animationMaster = animationMaster;
	}

	//returns a key for a given particle name
	//-1 if not found
	int getKeyFromName(string name) {
		unique_lock<mutex> lck(particleTypeLock);
		return particleKeys.count(name) ? particleKeys[name] : -1;
	}

	//Registers a particle type for use
	//returning an int of key for access, -1 on error
	int registerNewParticleType(string particleName, string animationSetName, int animation = 1) {
		unique_lock<mutex> lck(particleTypeLock);
		particleTypes[particleTypeAllocator] = ParticleType(animationSetName, animationMaster, animation);
		particleKeys[particleName] = particleTypeAllocator;

		return particleTypeAllocator++;
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

	//copies particles for rendering
	void copyParticles(vector<ParticleSpecification>& copy) {
		unique_lock<mutex> lck(particlesLock);
		for (auto& i : particles) {
			copy.push_back(i.getSpecification());
		}
	}

	//evaluates a vector of particles into a vector of boxes
	void evaluateParticleList(vector<ParticleSpecification>& particles, vector<BoxData>& boxes) {
		unique_lock<mutex> lck(particleTypeLock);
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

};

#endif