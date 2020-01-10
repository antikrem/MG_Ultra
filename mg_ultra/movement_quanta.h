#ifndef __MOVEMENT_QUANTA__
#define __MOVEMENT_QUANTA__

#include "cus_struct2.h"

#include "component_movement.h"

/*Building blocks for movemnt commands*/
class MovementQuanta {

public:
	virtual ~MovementQuanta() {
	}

	virtual bool isExecuting() = 0;

	virtual Point3 updateExecution(shared_ptr<ComponentMovement>, const Point3&) = 0;
};



#endif