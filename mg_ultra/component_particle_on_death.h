//spawn particles when killed by health
//or damage
#ifndef __COMPONENT_PARTICLE_ON_DEATH__
#define __COMPONENT_PARTICLE_ON_DEATH__

#include <atomic>
#include <tuple>

#include "component.h"
#include "scriptable_class.h"

#include "particle.h"

class ComponentParticleOnDeath : public Component, public ScriptableClass<ComponentParticleOnDeath> {
private:

	bool inPool = false;

	int key = -1;

	int count = 10;
	string particleKeyName;

	float radius = -1.0f;

	atomic<float> lowerVelocityFactor = 0;
	atomic<float> upperVelocityFactor = 1.0f;

	atomic<float> directionDeviation = 0.0f;

	atomic<float> explosionSpeed = 0.0f;

	ParticleTypeSpecification particleTypeSpecification;

	//sometimes, use an alternative position
	bool useAlternativePosition = false;
	Point3 alternativePosition = Point3(0.0f);

public:
	ComponentParticleOnDeath() {

	}

	ComponentParticleOnDeath(string particleKeyName) {
		this->particleKeyName = particleKeyName;
	}

	ComponentParticleOnDeath(string particleKeyName, int count) {
		this->particleKeyName = particleKeyName;
		this->count = count;
	}

	bool isRegistered() {
		return inPool;
	}

	void markIsRegistered() {
		inPool = true;
	}

	int getCount() {
		return count;
	}

	string getName() {
		return particleKeyName;
	}

	void setKey(int key) {
		this->key = key;
	}

	int getKey() {
		return key;
	}

	void setAllParameters(ParticleTypeSpecification particleTypeSpecification) {
		this->particleTypeSpecification = particleTypeSpecification;
	}

	void setAdjustedPosition(const Point3& alternativePosition) {
		useAlternativePosition = true;
		this->alternativePosition = alternativePosition;
	}

	bool isAdjustedPosition() {
		return useAlternativePosition;
	}

	Point3 getAdjustedPosition() {
		return alternativePosition;
	}

	ParticleTypeSpecification getSpec() {
		return particleTypeSpecification;
	}

	float getRadius() {
		return radius;
	}

	void setRadius(float radius) {
		this->radius = radius;
	}

	float getExplosionSpeed() {
		return explosionSpeed;
	}

	void setExplosionSpeed(float explosionSpeed) {
		this->explosionSpeed = explosionSpeed;
	}

	float getDirectionDeviation() {
		return directionDeviation;
	}

	void setDirectionDeviation(float directionDeviation) {
		this->directionDeviation =  directionDeviation;
	}

	float getMaxLifeDeviation() {
		return particleTypeSpecification.maxLifeDeviation;
	}

	void setVelocityFactorRange(float lower, float higher) {
		lowerVelocityFactor = lower;
		upperVelocityFactor = higher;
	}

	tuple<float, float> getVelocityFactorRange() {
		return make_tuple(lowerVelocityFactor.load(), upperVelocityFactor.load());
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentParticleOnDeath"].setClass(kaguya::UserdataMetatable<ComponentParticleOnDeath, Component>()
			.setConstructors<ComponentParticleOnDeath()>()
			.addFunction("set_velocity_range", &ComponentParticleOnDeath::setVelocityFactorRange)
			.addFunction("set_direction_deviation", &ComponentParticleOnDeath::setDirectionDeviation)
			.addFunction("set_explosion_speed", &ComponentParticleOnDeath::setExplosionSpeed)
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentParticleOnDeath, string>,
				ScriptableClass::create<ComponentParticleOnDeath, string, int>
			)
			.addStaticFunction("type", &getType<ComponentParticleOnDeath>)
			.addStaticFunction("cast", &Component::castDown<ComponentParticleOnDeath>)
		);
	}
};


#endif