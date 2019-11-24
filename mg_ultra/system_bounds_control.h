/*Will kill entities that go outside of the "gamebounds"*/
#ifndef __SYSTEM_BOUNDS_CONTROL__
#define __SYSTEM_BOUNDS_CONTROL__

#include "system.h"

#include "component_position.h"
#include "component_camera.h"
#include "component_no_bounds_control.h"

class SystemBoundsControl : public System {
	//if true, bounds are being held
	bool boundsEnforced = false;

	//get the current play space. halfed
	int xSpace, ySpace;

	//gradient of the camera
	float xGradient = 0;
	float yGradient = 0;

	//height of camera
	float height = 0;

	//padding x,y for culling, added to playspace
	//culling 
	int padding = 0;
public:
	SystemBoundsControl() {
		debugName = "s_bound_control";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentPosition>()
		);
	}

	bool executeSystem() override {
		//only execute if all these variables exist
		//and bounds are enforced
		if (registar->get("play_space_x", &xSpace) 
				&& registar->get("play_space_y", &ySpace)
				&& registar->get("culling_enable", &boundsEnforced)
				&& registar->get("culling_padding", &padding)
			) {
			xSpace = abs(xSpace/2);
			ySpace = abs(ySpace / 2);
			return boundsEnforced;
		}
		else {
			return false;
		}

	}

	void precycle(EntityPool* pool) override {
		//get the camera entity, to use 
		auto camera = pool->getCachedEnt(ETCamera);
		if (!camera) {
			boundsEnforced = false;
			return;
		}

		//need to compute scene height
		height = camera->getComponent<ComponentPosition>()->getPosition3().z;
		//calculate gradient
		xGradient = xSpace / height;
		yGradient = ySpace / height;

	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		//if bounds not enforced exit early
		if (!boundsEnforced) {
			return;
		}

		//ignore ents with nobounds
		if (getComponent<ComponentNoBoundsControl>(components)) {
			return;
		}

		auto cPosition = getComponent<ComponentPosition>(components);
		
		Point3 pos = cPosition->getPosition3();
		float depth = height + abs(pos.z);

		//check out of bounds
		if (
			//check x
			(abs(pos.x) > (xGradient * depth + padding)) ||
			//check y
			(abs(pos.y) > (yGradient * depth + padding))
			) {
			cPosition->killEntity();
		}

	}

};

#endif