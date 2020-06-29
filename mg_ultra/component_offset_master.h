/*This component causes an entity to be offset by the position of its master
and its change in position
optionally, its initial position value can be treated as an offset*/
#ifndef __COMPONENT_OFFSET_MASTER__
#define __COMPONENT_OFFSET_MASTER__

#include <atomic>

#include "cus_struct2.h"

#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

class ComponentOffsetMaster : public Component, public ScriptableClass<ComponentOffsetMaster> {
private:

	atomic<bool> initialised = false;

	atomic<bool> fullOffset = false;

	atomic<bool> disabled = false;

	atomic<Point3> masterLastPosition = Point3(0.0f);

public:
	ComponentOffsetMaster() {

	}


	//allows full offset to be specified
	//With full offset, this entitie's initial position will be fully offset
	//by the master entities position
	ComponentOffsetMaster(bool fullOffset) {
		this->fullOffset = fullOffset;
	}
	
	//allows ComponentOffsetMaster to be fully initialised by master position
	//on construction
	ComponentOffsetMaster(Point3 masterStartingOffset) {
		initialised = true;

		this->masterLastPosition = masterStartingOffset;
	}

	//returns new offset to be added to the position 
	Point3 updatePositionalOffset(Point3& masterPosition) {
		if (!initialised) {

			//initialise positions
			initialised = true;
			masterLastPosition = masterPosition;

			//if full offseted, need to return the masters position as offset
			return fullOffset ? masterPosition : Point3(0.0f);
		}
		else {
			if (!disabled) {
				Point3 newOffset = masterPosition - masterLastPosition.load();
				masterLastPosition = masterPosition;
				return newOffset;
			}
			else {
				return Point3(0.0f);
			}
			
		}

	}

	void disable() {
		disabled = true;
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentOffsetMaster"].setClass(kaguya::UserdataMetatable<ComponentOffsetMaster, Component>()
			.setConstructors<ComponentOffsetMaster()>()
			.addFunction("disable", &ComponentOffsetMaster::disable)
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentOffsetMaster>,
				ScriptableClass::create<ComponentOffsetMaster, bool>,
				ScriptableClass::create<ComponentOffsetMaster, Point3>
			)
			.addStaticFunction("type", &getType<ComponentOffsetMaster>)
			.addStaticFunction("cast", &Component::castDown<ComponentOffsetMaster>)
		);
	}
};

#endif