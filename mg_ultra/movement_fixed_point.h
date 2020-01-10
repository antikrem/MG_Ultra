#ifndef __MOVEMENT_FIXED_POINT__
#define __MOVEMENT_FIXED_POINT__

#include "movement_quanta.h"

class MovementFixedPoint : public MovementQuanta {
	Point3 position;

public:
	MovementFixedPoint(const Point3& position)
	: position(position) {
	}

	~MovementFixedPoint() {  
	}

	bool isExecuting() override {
		return false;
	}

	Point3 updateExecution(shared_ptr<ComponentMovement> componentMovement, const Point3& position) override  {
		return this->position;
	}
};

#endif