//Encapsulates a source of particles
#ifndef __PARTICLE_GENERATOR__
#define __PARTICLE_GENERATOR__

#include "particle.h"
#include "algorithm_ex.h"

#include "random_ex.h"

#include <mutex>
#include <atomic>
#include <vector>

class ParticleSpawner {
	atomic<int> particleKey = -1;

	//inate particle generation
	int particlesPerCycle = 0;

	//lock for particles
	mutex lock;
	//particles to create
	vector<Particle> particles;

	//some parameters for spawning particles
	atomic<Point3> position = Point3(0.0f);
	float positionDoubleDeviation = 1.0f;


	atomic<Point3> startingVelocity = Point3(0.0f);
	atomic<float> velocityDoubleDeviation = 0.0f;

	atomic<float> maxLifeDeviation = 1.0f;

public:
	//null particle spawner, will return -1 for key
	ParticleSpawner() {

	}

	//gets the key of this ParticleSpawner
	int getKey() {
		return particleKey;
	}

	//sets the key
	void setKey(int key) {
		this->particleKey = key;
	}

	//spawns count particle within bounds
	void spawnParticles(int count) {
		unique_lock<mutex> lck(lock);

		for (int i = 0; i < count; i++) {
			particles.push_back(
				Particle(
					particleKey,
					rand_ex::next_unif(1.0f, maxLifeDeviation),
					Point3(
						rand_ex::next_norm(position.load().x, positionDoubleDeviation / 2),
						rand_ex::next_norm(position.load().y, positionDoubleDeviation / 2),
						rand_ex::next_norm(position.load().z, positionDoubleDeviation / 2)
					),
					Point3(
						rand_ex::next_norm(startingVelocity.load().x, velocityDoubleDeviation / 2),
						rand_ex::next_norm(startingVelocity.load().y, velocityDoubleDeviation / 2),
						rand_ex::next_norm(startingVelocity.load().z, velocityDoubleDeviation / 2)
					)
				)

			);
		}

	}

	//sets the base velocity of particles
	void setStartingVelocity(const Point3& startingVelocity) {
		this->startingVelocity = startingVelocity;
	}

	//set the double deviation of position
	void setPositionDoubleDeviation(float doubleDeviation) {
		this->positionDoubleDeviation = doubleDeviation;
	}

	//sets maxlife deviation
	void setMaxLifeDeviation(float maxLifeDeviation) {
		this->maxLifeDeviation = maxLifeDeviation;
	}

	//updates this particle spawner
	void updateSpawner(vector<Particle>& particles, const Point3& position) {
		this->position = position;

		unique_lock<mutex> lck(lock);
		extend(particles, this->particles);
		this->particles.clear();
	}
};

#endif