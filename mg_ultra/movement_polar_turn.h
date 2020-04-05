#ifndef __MOVEMENT_POLAR_TURN__
#define __MOVEMENT_POLAR_TURN__

#include "movement_quanta.h"

class MovementPolarTurn : public MovementQuanta {
	float rate;
	int duration;
public:
	MovementPolarTurn(float rate, int duration) {
		this->rate = rate;
		this->duration = duration;
	}

	~MovementPolarTurn() {
	}

	bool isExecuting() override {
		return cycle <= duration;
	}

	Point3 updateExecution(shared_ptr<ComponentMovement> componentMovement, const Point3& position) override {
		if (!cycle) {
			componentMovement->setAngleChange(rate);
		}
		else if (cycle == duration) {
			componentMovement->setAngleChange(0);
		}
		return position;
	}
};

#endif