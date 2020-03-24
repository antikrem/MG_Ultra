/* Allows the object to magnetise to player
 */
#ifndef __COMPONENT_MAGNETISE_TO_PLAYER__
#define __COMPONENT_MAGNETISE_TO_PLAYER__

#include <atomic>
#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

class ComponentMagnetiseToPlayer : public Component, public ScriptableClass {
private:
	atomic<bool> magnetised = false;
	atomic<bool> initialised = false;

	float acceleration = 1.0;

public:
	ComponentMagnetiseToPlayer() {

	}

	ComponentMagnetiseToPlayer(float acceleration) {
		this->acceleration = acceleration;
	}

	void setMagnetise(bool magnetised) {
		this->magnetised = magnetised;
	}

	bool getMagnetise() {
		return magnetised;
	}

	void magnetise() {
		this->magnetised = true;
	}

	// Sets initialise to true and returns previous value
	bool markInitialised() {
		return initialised.load() ? true : (initialised = true, false);
	}

	float getAcceleration() {
		return acceleration;
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentMagnetiseToPlayer"].setClass(kaguya::UserdataMetatable<ComponentMagnetiseToPlayer, Component>()
			.setConstructors<ComponentMagnetiseToPlayer()>()
			.addFunction("set_magnetise", &ComponentMagnetiseToPlayer::setMagnetise)
			.addFunction("magnetise", &ComponentMagnetiseToPlayer::magnetise)
			.addStaticFunction("create", ScriptableClass::create<ComponentMagnetiseToPlayer, float>)
			.addStaticFunction("type", &getType<ComponentMagnetiseToPlayer>)
			.addStaticFunction("cast", &Component::castDown<ComponentMagnetiseToPlayer>)
		);
	}
};


#endif