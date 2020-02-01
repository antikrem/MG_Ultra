/*Allows an entity to cast a directional light*/
#ifndef __COMPONENT_DIRECTIONAL_LIGHT__
#define __COMPONENT_DIRECTIONAL_LIGHT__

#include "component.h"
#include "cus_struct2.h"

#include "scriptable_class.h"
#include "directional_light.h"

class ComponentDirectionalLight : public Component, public ScriptableClass {
	DirectionalLight directionLight;

public:
	ComponentDirectionalLight(const Point3& direction, const Point3& colour)
	: directionLight(direction, colour) {
	}

	ComponentDirectionalLight()
		: directionLight(Point3(0, 0, 1.0) , Point3(1.0f, 1.0f, 1.0f)) {
	}

	ComponentDirectionalLight(float x, float y, float z)
		: directionLight(Point3(x, y, z), Point3(1.0f, 1.0f, 1.0f)) {
	}

	ComponentDirectionalLight(float x, float y, float z, float r, float g, float b)
		: directionLight(Point3(x, y, z), Point3(r, g, b)) {
	}

	void update() {
		directionLight.update();
	}

	void setStrength(float current, float rate, float target) {
		directionLight.setStrength(current, rate, target);
	}

	float getStrength() {
		return directionLight.getStrength();
	}

	DirectionalData getDirectionData() {
		return directionLight.getData();
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentDirectionalLight"].setClass(kaguya::UserdataMetatable<ComponentDirectionalLight, Component>()
			.setConstructors<ComponentDirectionalLight()>()
			.addFunction("setStrength", &ComponentDirectionalLight::setStrength)
			.addFunction("get_strength", &ComponentDirectionalLight::getStrength)
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentDirectionalLight, float, float, float>,
				ScriptableClass::create<ComponentDirectionalLight, float, float, float, float, float, float>
			)
			.addStaticFunction("type", &getType<ComponentDirectionalLight>)
			.addStaticFunction("cast", &Component::castDown<ComponentDirectionalLight>)
		);
	}
};

#endif