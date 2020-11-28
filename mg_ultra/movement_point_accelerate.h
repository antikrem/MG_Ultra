#ifndef __MOVEMENT_POINT_ACCELERATE__
#define __MOVEMENT_POINT_ACCELERATE__

#include "movement_quanta.h"

class MovementPointAccelerate : public MovementQuanta {
	Point3 acceleration;
	bool firstTick = true;

public:
	MovementPointAccelerate(const Point3& acceleration)
		: acceleration(acceleration) {
	}

	~MovementPointAccelerate() {
	}

	bool isExecuting() override {
		return firstTick;
	}

	Point3 updateExecution(shared_ptr<ComponentMovement> componentMovement, const Point3& position) override {
		if (firstTick) {
			firstTick = false;
			
			componentMovement->setAcceleration(acceleration);
		}

		return position;
	}

};

#endif