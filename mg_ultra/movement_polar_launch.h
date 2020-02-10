#ifndef __MOVEMENT_POLAR_LAUNCH__
#define __MOVEMENT_POLAR_LAUNCH__

#include "movement_quanta.h"

class MovementPolarLaunch : public MovementQuanta {
	Point2 impulse;

public:
	MovementPolarLaunch(const Point2& impulse)
		: impulse(impulse) {
	}

	~MovementPolarLaunch() {
	}

	bool isExecuting() override {
		return false;
	}

	Point3 updateExecution(shared_ptr<ComponentMovement> componentMovement, const Point3& position) override {
		componentMovement->setSpeed(impulse.x);
		componentMovement->setAngle(impulse.y);
		return position;
	}
};

#endif