#ifndef __MOVEMENT_POLAR_ACCELERATE_TO__
#define __MOVEMENT_POLAR_ACCELERATE_TO__

#include "movement_quanta.h"

class MovementPolarAccelerateTo : public MovementQuanta {
	float endingSpeed;
	int duration;
	bool firstTick = true;
	int cycle = -1;
public:
	MovementPolarAccelerateTo(float endingSpeed, int duration) {
		this->endingSpeed = endingSpeed;
		this->duration = duration;
	}

	~MovementPolarAccelerateTo() {
	}

	bool isExecuting() override {
		return cycle <= duration;
	}

	Point3 updateExecution(shared_ptr<ComponentMovement> componentMovement, const Point3& position) override {
		cycle++;

		if (firstTick) {
			firstTick = false;
			float rate = (endingSpeed - componentMovement->getSpeed()) / duration;

			componentMovement->setSpeedChange(rate);
		}

		else if (cycle == duration) {
			componentMovement->setSpeedChange(0);
		}

		return position;
	}
};

#endif