/*Adds a force field around an entity which
will push particles away*/
#ifndef __COMPONENT_FORCE_APPLIER__
#define __COMPONENT_FORCE_APPLIER__

#include "force_applier.h"

#include "component.h"
#include "constants.h"
#include "scriptable_class.h"


class ComponentForceApplier : public Component, public ScriptableClass<ComponentForceApplier> {
private:
	ForceApplier forceApplier;

public:
	ComponentForceApplier() {
	}

	ComponentForceApplier(float cutdistance, float peak) {
		forceApplier.cutdistance = cutdistance;
		forceApplier.peak = peak;
	}

	void setPeak(float peak) {
		forceApplier.peak = peak;
	}

	void setCutdistance(float cutdistance) {
		forceApplier.cutdistance = cutdistance;
	}

	void setCutThreashold(float cutthreshold) {
		forceApplier.cutoff = cutthreshold;
	}

	//sets the position
	void setPosition(const Point3& position) {
		forceApplier.position = position;
	}

	//returns the internal forceApplier
	ForceSpecification getForceApplier() {
		return forceApplier.getSpecification();
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentForceApplier"].setClass(kaguya::UserdataMetatable<ComponentForceApplier, Component>()
			.setConstructors<ComponentForceApplier()>()
			.addFunction("set_peak", &ComponentForceApplier::setPeak)
			.addFunction("set_cut_distance", &ComponentForceApplier::setCutdistance)
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentForceApplier>,
				ScriptableClass::create<ComponentForceApplier, float, float>
			)
			.addStaticFunction("type", &getType<ComponentForceApplier>)
			.addStaticFunction("cast", &Component::castDown<ComponentForceApplier>)
		);
	}
};

#endif