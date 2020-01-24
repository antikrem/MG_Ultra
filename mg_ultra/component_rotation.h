/*Indicates the entity has a more complex spawning system*/
#ifndef __COMPONENT_ROTATION__
#define __COMPONENT_ROTATION__

#include <atomic>
#include "component.h"

#include "scriptable_class.h"

class ComponentRotation : public Component, public ScriptableClass {

	atomic<bool> faceMovement = false;

	atomic<float> rotation = 0.0f;

public:

	ComponentRotation() {
	}

	ComponentRotation(bool faceMovement) {
		this->faceMovement = faceMovement;
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

	void updateRotation(float rotation) {
		this->rotation = rotation;
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentRotation"].setClass(kaguya::UserdataMetatable<ComponentRotation, Component>()
			.setConstructors<ComponentRotation()>()
			.addFunction("set_angle", &ComponentRotation::setRotation)
			.addOverloadedFunctions(
				"create",
				&ScriptableClass::create<ComponentRotation>,
				&ScriptableClass::create<ComponentRotation, bool>
			)
			.addStaticFunction("type", &getType<ComponentRotation>)
			.addStaticFunction("cast", &Component::castDown<ComponentRotation>)
		);
	}
};

#endif