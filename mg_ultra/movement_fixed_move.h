#ifndef __MOVEMENT_FIXED_MOVE__
#define __MOVEMENT_FIXED_MOVE__

#include "movement_quanta.h"

//allows for smoothed movement between points
class MovementFixedMove : public MovementQuanta {
	//target position
	Point3 targetPosition;

	int duration;

public:
	MovementFixedMove(const Point3& position, int duration)
		: targetPosition(position) {
		this->duration = duration;
	}

	~MovementFixedMove() {
	}

	bool isExecuting() override {
		return false;
	}

	Point3 updateExecution(shared_ptr<ComponentMovement> componentMovement, const Point3& position) override {
		componentMovement->pinVelocity();
		componentMovement->pushVelocityToPolar();
		Point3 temp = position;
		Point2 a = temp.directionTo(targetPosition).getXY();
		float b = a.getAng();
		componentMovement->setAngle(temp.directionTo(targetPosition).getXY().getAng());

		float distance = temp.distanceTo(targetPosition);

		componentMovement->setSpeedChange(
			(2 * (distance - componentMovement->getSpeed() * duration)) / SQUARE(duration)
		);


		return position;
	}
};

#endif