/*A component added for particle generation*/
#ifndef __COMPONENT_PARTICLE_SPAWNER__
#define __COMPONENT_PARTICLE_SPAWNER__

#include "component.h"
#include "scriptable_class.h"

#include "particle_spawner.h"

class ComponentParticle : public Component, public ScriptableClass {
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
	void spawnParticlesUniformly(float density) {
		particleSpawner.spawnParticlesUniformly(density);
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

	void registerToLua(kaguya::State& state) override {
		state["ComponentParticle"].setClass(kaguya::UserdataMetatable<ComponentParticle, Component>()
			.setConstructors<ComponentParticle()>()
			.addFunction("spawn", &ComponentParticle::spawnParticles)
			.addFunction("spawn_uniformly", &ComponentParticle::spawnParticlesUniformly)
			.addFunction("set_velocity", &ComponentParticle::l_setStartingVelocity)
			.addFunction("set_position_deviation", &ComponentParticle::setPositionDeviation)
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