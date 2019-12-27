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
	float twoDeviations = 1.0f;


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

	//spawns one particle within bounds
	void spawnParticles(int count) {
		unique_lock<mutex> lck(lock);

		for (int i = 0; i < count; i++) {
			particles.push_back(
				Particle(
					particleKey,
					Point3(
						rand_ex::next_norm(position.load().x, twoDeviations / 2),
						rand_ex::next_norm(position.load().y, twoDeviations / 2),
						rand_ex::next_norm(position.load().z, twoDeviations / 2)
					)
				)
			);
		}
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