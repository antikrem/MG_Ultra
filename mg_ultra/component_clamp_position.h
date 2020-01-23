/*A Component that can limit the position of an entity*/
#ifndef __COMPONENT_CLAMP_POSITION__
#define __COMPONENT_CLAMP_POSITION__

#include <atomic>

#include "cus_struct2.h"

#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

#include "math_ex.h"

class ComponentClampPosition : public Component, public ScriptableClass {
private:
	Point3 center;
	Point3 dimensions;

public:
	ComponentClampPosition()
	: center(0.0f), dimensions(0.0f) {
	}

	ComponentClampPosition(float w, float h, float d)
	: center(0.0f), dimensions(w, h, d) {
	}

	ComponentClampPosition(float w, float h)
		: center(0.0f), dimensions(w, h, INFINITY) {
	}

	ComponentClampPosition(float w, float h, float d, float x, float y, float z)
		: center(x, y, z), dimensions(w, h, d) {
	}

	Point3 getCenter() {
		return center;
	}

	Point3 getDimensions() {
		return dimensions;
	}

	//applies clamping
	Point3 applyClamp(Point3 pos) {
		return Point3(
			math_ex::clamp(pos.x, center.x - dimensions.x, center.x + dimensions.x),
			math_ex::clamp(pos.y, center.y - dimensions.y, center.y + dimensions.y),
			math_ex::clamp(pos.z, center.z - dimensions.z, center.z + dimensions.z)
		);
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentClampPosition"].setClass(kaguya::UserdataMetatable<ComponentClampPosition, Component>()
			.setConstructors<ComponentClampPosition()>()
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentClampPosition, float, float>,
				ScriptableClass::create<ComponentClampPosition, float, float, float>,
				ScriptableClass::create<ComponentClampPosition, float, float, float, float, float, float>
			)
			.addStaticFunction("type", &getType<ComponentClampPosition>)
			.addStaticFunction("cast", &Component::castDown<ComponentClampPosition>)
		);
	}
};


#endif