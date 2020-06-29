/*Indicates the entity has a more complex spawning system*/
#ifndef __COMPONENT_ROTATION__
#define __COMPONENT_ROTATION__

#include <atomic>
#include "component.h"

#include "scriptable_class.h"

class ComponentRotation : public Component, public ScriptableClass<ComponentRotation> {

	atomic<bool> faceMovement = false;

	atomic<float> rotation = 0.0f;

	atomic<float> speed = 0.0f;

public:

	ComponentRotation() {
		faceMovement = true;
	}

	ComponentRotation(bool faceMovement) {
		this->faceMovement = faceMovement;
	}

	ComponentRotation(float rotation) {
		this->rotation = rotation;
	}

	ComponentRotation(float rotation, float speed) {
		this->rotation = rotation;
		this->speed = speed;
	}

	void setFaceMovement(bool faceMovement) {
		this->faceMovement = faceMovement;
	}

	bool isFaceMovement() {
		return faceMovement;
	}

	void setRotation(float rotation) {
		faceMovement = false;
		this->rotation = rotation;
	}

	float getRotation() {
		return rotation;
	}

	void updateRotation(float rotation) {
		this->rotation = rotation;
	}

	void setSpeed(float speed) {
		this->speed = speed;
	}

	float getSpeed() {
		return speed;
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentRotation"].setClass(kaguya::UserdataMetatable<ComponentRotation, Component>()
			.setConstructors<ComponentRotation()>()
			.addFunction("set_angle", &ComponentRotation::setRotation)
			.addFunction("get_angle", &ComponentRotation::getRotation)
			.addFunction("set_speed", &ComponentRotation::setSpeed)
			.addFunction("get_speed", &ComponentRotation::getSpeed)
			.addOverloadedFunctions(
				"create",
				&ScriptableClass::create<ComponentRotation>,
				&ScriptableClass::create<ComponentRotation, float>,
				&ScriptableClass::create<ComponentRotation, float, float>
			)
			.addStaticFunction("type", &getType<ComponentRotation>)
			.addStaticFunction("cast", &Component::castDown<ComponentRotation>)
		);
	}
};

#endif