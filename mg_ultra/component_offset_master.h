/**
 * This component causes an entity to be offset by the position of its master
 * and its change in position
 * optionally, its initial position value can be treated as an offset
 */
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

	shared_ptr<Entity> master = nullptr;

	shared_ptr<ComponentPosition> componentPosition = nullptr;

public:
	ComponentOffsetMaster() {

	}


	// Allows full offset to be specified
	// With full offset, this entitie's initial position will be fully offset
	// by the master entities position
	ComponentOffsetMaster(bool fullOffset) {
		this->fullOffset = fullOffset;
	}
	
	// Sets master and component position
	void initialise(shared_ptr<Entity> master, shared_ptr<ComponentPosition> componentPosition) {
		this->master = master;
		this->componentPosition = componentPosition;

		masterLastPosition = master->getComponent<ComponentPosition>()->getPosition3();
		if (fullOffset) {
			componentPosition->addPosition(masterLastPosition);
		}

		initialised = true;
	}

	// Returns new offset to be added to the position 
	void updatePositionalOffset() {

		if (!initialised) {
			return;
		}

		if (!disabled) {
			if (!master->getFlag) {
				disable();
				master = nullptr;
			}

			Point3 masterPosition = master->getComponent<ComponentPosition>()->getPosition3();
			Point3 newOffset = masterPosition - masterLastPosition.load();
			masterLastPosition = masterPosition;
			componentPosition->addPosition(newOffset);
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
				ScriptableClass::create<ComponentOffsetMaster, bool>
			)
			.addStaticFunction("type", &getType<ComponentOffsetMaster>)
			.addStaticFunction("cast", &Component::castDown<ComponentOffsetMaster>)
		);
	}
};

#endif