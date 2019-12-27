//Encapsulates a single particle
//most particle information is tied to external classes
#ifndef __PARTICLE__
#define __PARTICLE__

#include <atomic>

#include "cus_struct2.h"

//Minimal specification required for a particle
struct ParticleSpecification {
	float lifetime;
	float x;
	float y;
	float z;
	int particleKey;

	ParticleSpecification(float lifetime, float x, float y, float z, int particleKey) {
		this->lifetime = lifetime;

		this->x = x;
		this->y = y;
		this->z = z;

		this->particleKey = particleKey;
	}
};

struct Particle {
	bool active = true;

	atomic<float> lifetime = 0.0f;

	Point3 position;
	Point3 velocity;
	Point3 acceleration;

	int particleKey;

	Particle(int key, const Point3& position, const Point3& velocity = Point3(0.0f), const Point3& acceleration = Point3(0.0f) ) 
	: position(position), velocity(velocity), acceleration(acceleration) {
		this->particleKey = key;
	}

	Particle(const Particle& particle)
	: position(particle.position), velocity(particle.velocity), acceleration(particle.acceleration) {
		this->active = particle.active;
		
		this->lifetime = particle.lifetime.load();

		this->particleKey = particle.particleKey;
	}

	Particle& operator=(const Particle& particle) {
		this->active = particle.active;

		this->lifetime = particle.lifetime.load();

		position = particle.position;
		velocity = particle.velocity;
		acceleration = particle.acceleration;

		this->particleKey = particle.particleKey;

		return *this;
	}

	//updates a particle
	//time factor refers to how many cycles worth of updates are required
	void update(float scalarFactor = 1.0f) {
		velocity = velocity + acceleration * scalarFactor;
		position = position + velocity * scalarFactor;

		lifetime = lifetime + scalarFactor;
	}

	//returns a specification for this particle
	ParticleSpecification getSpecification() {
		return ParticleSpecification(lifetime, position.x, position.y, position.z, particleKey);
	}
};

#endif