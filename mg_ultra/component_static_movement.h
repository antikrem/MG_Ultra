#ifndef __COMPONENT_STATIC_MOVEMENT__
#define __COMPONENT_STATIC_MOVEMENT__

#include "movement_commander.h"

#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

class ComponentStaticMovement : public Component, public ScriptableClass<ComponentStaticMovement> {
private:
	MovementCommander movementCommander;
public:

	ComponentStaticMovement() {

	}

	void addFixedPoint(int time, const Point3& position) {
		movementCommander.addFixedPoint(time, position);
	}

	void ll_addFixedPoint(int time, float x, float y) {
		movementCommander.addFixedPoint(time, Point3(x, y, 0.0f));
	}

	void lll_addFixedPoint(int time, float x, float y, float z) {
		movementCommander.addFixedPoint(time, Point3(x, y, z));
	}

	void chainFixedMove(int time, const Point3& position) {
		movementCommander.chainFixedMove(time, position);
	}

	void ll_chainFixedMove(int time, float x, float y) {
		movementCommander.chainFixedMove(time, Point3(x, y, 0.0f));
	}

	void lll_chainFixedMove(int time, float x, float y, float z) {
		movementCommander.chainFixedMove(time, Point3(x, y, z));
	}

	void chainSmoothPoint(vector<float> input) {
		movementCommander.chainSmoothPoints(input);
	}

	void addPolarLaunch(int time, float mag, float ang) {
		movementCommander.addPolarLaunch(time, mag, ang);
	}

	void addPolarturn(int time, int duration, float totalAngle) {
		movementCommander.addPolarturn(time, duration, totalAngle);
	}

	void addPolarAccelerateTo(int time, int duration, float endingSpeed) {
		movementCommander.addPolarAccelerateTo(time, duration, endingSpeed);
	}

	void addPointAccelerateTo(int time, Point3 acceleration) {
		movementCommander.addPointAccelerateTo(time, acceleration);
	};

	void ll_addPointAccelerateTo(int time, float ax, float ay) {
		movementCommander.addPointAccelerateTo(time, {ax, ay, 0});
	};

	Point3 getUpdatedPosition(shared_ptr<ComponentMovement> componentMovement, const Point3& position) {
		return movementCommander.getUpdatedPosition(componentMovement, position);
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentStaticMovement"].setClass(kaguya::UserdataMetatable<ComponentStaticMovement, Component>()
			.setConstructors<ComponentStaticMovement()>()
			.addOverloadedFunctions(
				"add_fixed_point",
				&ComponentStaticMovement::ll_addFixedPoint,
				&ComponentStaticMovement::lll_addFixedPoint
			)
			.addOverloadedFunctions(
				"chain_fixed_move",
				&ComponentStaticMovement::ll_chainFixedMove,
				&ComponentStaticMovement::lll_chainFixedMove
			)
			.addFunction(
				"chain_smooth_points",
				&ComponentStaticMovement::chainSmoothPoint
			)
			.addFunction(
				"add_polar_launch",
				&ComponentStaticMovement::addPolarLaunch
			)
			.addFunction(
				"add_polar_turn",
				&ComponentStaticMovement::addPolarturn
			)
			.addFunction(
				"add_polar_accelerate_to",
				&ComponentStaticMovement::addPolarAccelerateTo
			)
			.addFunction(
				"add_point_accelerate",
				&ComponentStaticMovement::ll_addPointAccelerateTo
			)
			.addOverloadedFunctions(
				"create",
				&ScriptableClass::create<ComponentStaticMovement>
			)
			.addStaticFunction("type", &getType<ComponentStaticMovement>)
			.addStaticFunction("cast", &Component::castDown<ComponentStaticMovement>)
		);
	}
};

#endif