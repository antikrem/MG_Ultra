/*A component added for particle generation*/
#ifndef __COMPONENT_PARTICLE_SPAWNER__
#define __COMPONENT_PARTICLE_SPAWNER__

#include "component.h"
#include "scriptable_class.h"

#include "particle_spawner.h"

class ComponentParticle : public Component, public ScriptableClass<ComponentParticle> {
private:
	//ambient spawning of particles
	atomic<float> ambientSpawning = 0.0f;
	//amount to spawn
	float ambientQueued = 0.0f;

	string particleKeyName;
	ParticleSpawner particleSpawner;

public:
	ComponentParticle() {

	}

	ComponentParticle(string particleKeyName) {
		this->particleKeyName = particleKeyName;
	}

	//updates particle spawner
	void updateSpawner(vector<Particle>& particles, const Point3& position) {
		ambientQueued += ambientSpawning;


		particleSpawner.updateSpawner(particles, position);
	}

	//sets particle name by string
	void setName(string name) {
		int key = g_particles::getParticleType(name);
		if (key > 0) {
			particleSpawner.setKey(key);
			auto spec = g_particles::getTypeSpec(key);
			setAllParameters(spec);
		}
	}

	//get string name
	string getKeyName() {
		return particleKeyName;
	}

	//gets key
	int getKey() {
		return particleSpawner.getKey();
	}

	//sets the key
	void setKey(int key) {
		particleSpawner.setKey(key);
	}

	//sets all the parameters
	void setAllParameters(ParticleTypeSpecification& spec) {
		particleSpawner.setAllParameters(spec);
	}

	//sets featherness values
	void setFeathernessParameters(float feathernessMean, float feathernessDeviation) {
		particleSpawner.setFeathernessParameters(feathernessMean, feathernessDeviation);
	}

	//sets weight values
	void setWeightParameters(float weightMean, float weightDeviation) {
		particleSpawner.setWeightParameters(weightMean, weightDeviation);
	}

	//spawn particles relative to entity
	void spawnParticles(int count) {
		particleSpawner.spawnParticles(count);
	}

	//spawn particles relative to entity
	void spawnParticlesAt(float x, float y, float z) {
		particleSpawner.spawnParticlesAt(Point3(x, y, z));
	}

	//spawn particles relative to entity
	void spawnParticlesUniformly(int count) {
		particleSpawner.spawnParticlesUniformly(count);
	}

	//set starting velocity
	void setStartingVelocity(const Point3& startingVelocity) {
		particleSpawner.setStartingVelocity(startingVelocity);
	}

	void l_setStartingVelocity(float x, float y, float z) {
		particleSpawner.setStartingVelocity(Point3(x, y, z));
	}

	//sets the double deviation more accuratly
	void setPositionDeviation(float doubleDeviation) {
		particleSpawner.setPositionDoubleDeviation(doubleDeviation);
	}
	
	//sets life deviation
	void setMaxLifeDeviation(float maxLifeDeviation) {
		particleSpawner.setMaxLifeDeviation(maxLifeDeviation);
	}

	//set rotate to face
	void setRotateToFace(bool rotateToFace) {
		particleSpawner.setRotateToFace(rotateToFace);
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentParticle"].setClass(kaguya::UserdataMetatable<ComponentParticle, Component>()
			.setConstructors<ComponentParticle()>()
			.addFunction("spawn", &ComponentParticle::spawnParticles)
			.addFunction("spawn_at", &ComponentParticle::spawnParticlesAt)
			.addFunction("spawn_uniformly", &ComponentParticle::spawnParticlesUniformly)
			.addFunction("set_velocity", &ComponentParticle::l_setStartingVelocity)
			.addFunction("set_position_deviation", &ComponentParticle::setPositionDeviation)
			.addFunction("use_type", &ComponentParticle::setName)
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentParticle, string>
			)
			.addStaticFunction("type", &getType<ComponentParticle>)
			.addStaticFunction("cast", &Component::castDown<ComponentParticle>)
		);
	}
};


#endif