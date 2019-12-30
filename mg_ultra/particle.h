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

	float lifetime = 0.0f;

	float featherness = 0.1f;

	Point3 position;
	Point3 velocity;

	Point3 momentum;

	int particleKey;

	Particle(int key, const Point3& position, const Point3& startingVelocity = Point3(0.0f) ) 
	: position(position), velocity(startingVelocity), momentum(startingVelocity) {
		this->particleKey = key;
	}

	//updates a particle
	//time factor refers to how many cycles worth of updates are required
	void update(const Point3 mommentum, float scalarFactor = 1.0f) {
		this->momentum = mommentum;

		float amendedScalarFactor = min(1.0f, scalarFactor * featherness);
		velocity = velocity * (1.0f - amendedScalarFactor) + momentum * amendedScalarFactor;
		position = position + velocity * scalarFactor;

		lifetime = lifetime + scalarFactor;
	}

	//returns a specification for this particle
	ParticleSpecification getSpecification() {
		return ParticleSpecification(lifetime, position.x, position.y, position.z, particleKey);
	}
};

#endif