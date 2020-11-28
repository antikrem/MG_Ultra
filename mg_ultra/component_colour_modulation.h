/**
 * A Component that can modulate the colour of an attached graphics element
 */
#ifndef __COMPONENT_COLOUR_MODULATOR__
#define __COMPONENT_COLOUR_MODULATOR__

#include <atomic>
#include <shared_mutex>

#include "component.h"
#include "constants.h"


#include "scriptable_class.h"

class ComponentColourModulation : public Component, public ScriptableClass<ComponentColourModulation> {
private:
	atomic<Point3> modulation;
	atomic<float> strength = 1.0;

public:
	ComponentColourModulation() 
		: modulation(Point3(0.0f, 0.5f, 0.5f)) {
		strength = 0.0;
	}

	ComponentColourModulation(float value) 
		: modulation(Point3(value, 0.5f, 0.5f)) {
	}

	ComponentColourModulation(float h, float s, float l) 
		: modulation(Point3(h, s, l)) {
	}

	ComponentColourModulation(Point3 value) 
		: modulation(value) {
	}


	// Sets current modulation
	void setModulation(float value) {
		modulation = Point3(value);
	}

	// Sets current modulation
	void setModulation(float h, float s, float l) {
		modulation = Point3(h, s, l);
	}

	// Wrapper for setting current modulation
	void l_setModulation(float h, float s, float l) {
		setModulation(h, s, l);
	}

	// Get modulation colour
	Point3 getModulation() {
		return modulation;
	}

	// Fills in paramters
	void populate(float* strength, float* hsl) {
		*strength = this->strength.load();
		Point3 modulation = this->modulation.load();
		hsl[0] = modulation.x;
		hsl[1] = modulation.y;
		hsl[2] = modulation.z;
	}

	// Get modulation colour
	tuple<float, float, float> l_getModulation() {
		Point3 value = modulation;
		return make_tuple(value.x, value.y, value.z);
	}

	// Get hue
	float getHue() {
		return modulation.load().x;
	}

	// Set hue
	void setHue(float hue) {
		Point3 modulation = getModulation();
		modulation.x = hue;
		this->modulation.store(modulation);
	}

	static void registerToLua(kaguya::State& state) {
		auto a = getType<ComponentColourModulation>();
		state["ComponentColourModulation"].setClass(kaguya::UserdataMetatable<ComponentColourModulation, Component>()
			.setConstructors<ComponentColourModulation()>()
			.addFunction("set_modulation", &ComponentColourModulation::l_setModulation)
			.addFunction("get_modulation", &ComponentColourModulation::l_getModulation)
			.addFunction("set_hue", &ComponentColourModulation::setHue)
			.addFunction("get_hue", &ComponentColourModulation::getHue)
			.addOverloadedFunctions(
				"create",
				&ScriptableClass::create<ComponentColourModulation>,
				&ScriptableClass::create<ComponentColourModulation, float>,
				&ScriptableClass::create<ComponentColourModulation, float, float, float>
			)
			.addStaticFunction("type", &getType<ComponentColourModulation>)
			.addStaticFunction("cast", &Component::castDown<ComponentColourModulation>)
		);
	}
};


#endif