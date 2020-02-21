//Organises particle managing
#ifndef __PARTICLE_MASTER__
#define __PARTICLE_MASTER__

#include <string>
#include <vector>
#include <map>
#include <tuple>
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

	//returns rotate to face
	bool getRotateToFace(int key) {
		shared_lock<shared_mutex> lck(particleTypeLock);
		return particleTypes.count(key)
			? particleTypes[key].rotateToFace
			: false;
	}

	//returns a tuple of featherness parameters
	tuple<float, float> getFeathernessParameters(int key) {
		shared_lock<shared_mutex> lck(particleTypeLock);
		return particleTypes.count(key) 
			? make_tuple(particleTypes[key].feathernessMean, particleTypes[key].feathernessDeviation) 
			: make_tuple(0.1f, 0.01f);
	}

	//returns a tuple of weight parameters
	tuple<float, float> getWeightParameters(int key) {
		shared_lock<shared_mutex> lck(particleTypeLock);
		return particleTypes.count(key)
			? make_tuple(particleTypes[key].weightMean, particleTypes[key].weightDeviation)
			: make_tuple(0.1f, 0.01f);
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
		unique_lock<shared_mutex> lck(particleTypeLock);
		if (particleKeys.count(particleName)) {
			particleTypes[particleKeys[particleName]].setBloomFactor(lightSensitivity);
		}
		else {
			err::logMessage("PARTICLE: Was not able to find named particle " + particleName);
		}
	}

	//modifies named particle type's max life deviation
	void setParticleMaxLifeDeviation(string particleName, float maxLifeDeviation) {
		unique_lock<shared_mutex> lck(particleTypeLock);
		if (particleKeys.count(particleName)) {
			particleTypes[particleKeys[particleName]].maxLifeDeviation = maxLifeDeviation;
		}
		else {
			err::logMessage("PARTICLE: Was not able to find named particle " + particleName);
		}
	}

	//modifies named particle type's bounding box response
	void setParticleTypeResponse(string particleName, ParticleBoxResponse response) {
		unique_lock<shared_mutex> lck(particleTypeLock);
		if (particleKeys.count(particleName)) {
			particleTypes[particleKeys[particleName]].boxResponse = response;
		}
		else {
			err::logMessage("PARTICLE: Was not able to find named particle " + particleName);
		}
	}

	//modifies named particle type's bounding box half dimension
	void setParticleTypeBoxDimension(string particleName, const Point3& dimension) {
		unique_lock<shared_mutex> lck(particleTypeLock);
		if (particleKeys.count(particleName)) {
			particleTypes[particleKeys[particleName]].boundingBoxDimension = dimension;
		}
		else {
			err::logMessage("PARTICLE: Was not able to find named particle " + particleName);
		}
	}

	//modifies named particle type's bounding box half dimension
	void setParticleTypeBoxCenter(string particleName, const Point3& center) {
		unique_lock<shared_mutex> lck(particleTypeLock);
		if (particleKeys.count(particleName)) {
			particleTypes[particleKeys[particleName]].boundingBoxPosition = center;
		}
		else {
			err::logMessage("PARTICLE: Was not able to find named particle " + particleName);
		}
	}

	//modifies named particle type's rotateToFace
	void setParticleTypeRotateToFace(string particleName, bool rotateToFace) {
		unique_lock<shared_mutex> lck(particleTypeLock);
		if (particleKeys.count(particleName)) {
			particleTypes[particleKeys[particleName]].rotateToFace = rotateToFace;

		}
		else {
			err::logMessage("PARTICLE: Was not able to find named particle " + particleName);
		}
	}

	//modifies named particle type's feather parameters
	void setParticleTypeFeatherness(string particleName, float featherMean, float featherDeviation) {
		unique_lock<shared_mutex> lck(particleTypeLock);
		if (particleKeys.count(particleName)) {
			particleTypes[particleKeys[particleName]].feathernessMean = featherMean;
			particleTypes[particleKeys[particleName]].feathernessDeviation = featherDeviation;
		}
		else {
			err::logMessage("PARTICLE: Was not able to find named particle " + particleName);
		}
	}

	//modifies named particle type's weight parameters
	void setParticleTypeWeight(string particleName, float weightMean, float weightDeviation) {
		unique_lock<shared_mutex> lck(particleTypeLock);
		if (particleKeys.count(particleName)) {
			particleTypes[particleKeys[particleName]].weightMean = weightMean;
			particleTypes[particleKeys[particleName]].weightDeviation = weightDeviation;
		}
		else {
			err::logMessage("PARTICLE: Was not able to find named particle " + particleName);
		}
	}

	//gets half particle box size
	Point3 getBoundingBoxSize(int key) {
		unique_lock<shared_mutex> lck(particleTypeLock);
		if (particleTypes.count(key)) {
			return particleTypes[key].boundingBoxDimension;
		}
		else {
			err::logMessage("PARTICLE: Was not able to find particle key " + key);
			return Point3(0);
		}
	}

	//gets particle box center
	Point3 getBoundingBoxCenter(int key) {
		unique_lock<shared_mutex> lck(particleTypeLock);
		if (particleTypes.count(key)) {
			return particleTypes[key].boundingBoxPosition;
		}
		else {
			err::logMessage("PARTICLE: Was not able to find particle key " + key);
			return Point3(0);
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
			auto& particle = particles[i];

			Point3 mommentum(0);
			{
				unique_lock<mutex> lck(forceSpecificationLock);
				Point3 pos = particle.position;
				for (auto& i : forceSpecifications) {
					mommentum += computeMommentum(i, pos);
				}
			}

			particle.update(wind, mommentum, (float)factor);

			//do particle type checking
			{
				unique_lock<shared_mutex> lck(particleTypeLock);
				//check for a box response
				auto& type = particleTypes[particle.particleKey];

				switch (type.boxResponse) {
					//do nothing of break
				case ParticleBoxResponse::Nothing:
					break;

					//delete on delete
				case ParticleBoxResponse::Delete:
					if (!type.checkBoundingBox(particle.position)) {
						particle.active = false;
					}
					break;

					//wrap on particle into box
				case ParticleBoxResponse::Wrap:
					type.wrapPosition(particle);
					break;
				}
			}
		}
	}

	//gets max life deviation from a particle type
	float getMaxLifeDeviation(int key) {
		unique_lock<shared_mutex> lck(particleTypeLock);
		if (particleTypes.count(key)) {
			return particleTypes[key].maxLifeDeviation;
		}
		else {
			return 1.0f;
		}
	}

	//clears all particle 
	int clearParticles() {
		unique_lock<mutex> lck(particlesLock);
		int particleSize = (int)particles.size();
		particles.clear();
		return particleSize;
	}

	//gets the specification for a given particle type
	ParticleTypeSpecification getTypeSpecification(int key) {
		if (particleTypes.count(key)) {
			return particleTypes[key].getParticleTypeSpecification();
		}
		else {
			return ParticleTypeSpecification();
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

	//Type of response to a particle falling out of range
	void updateTypeResponse(string particleName, int response);

	//modifies named particle type's bounding box half dimension
	void updateBoxDimension(string particleName, float x, float y, float z);

	//modifies named particle type's bounding box half dimension
	void updateBoxCenter(string particleName, float x, float y, float z);

	//updates rotate parameter
	void updateRotateToFace(string particleName, bool rotateToFace);

	//updates featherness of a particle type
	void updateFeatherness(string particleName, float featherMean, float featherDeviation);

	//updates weight of a particle type
	void updateWeight(string particleName, float weightMean, float weightDeviation);

	//clears all particles in the engine
	//returns how many particles were deleted
	int clearParticles();
}



#endif