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

	//spawn one particle
	void spawnParticles(int count) {
		particleSpawner.spawnParticles(count);
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentParticle"].setClass(kaguya::UserdataMetatable<ComponentParticle, Component>()
			.setConstructors<ComponentParticle()>()
			.addFunction("spawn", &ComponentParticle::spawnParticles)
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