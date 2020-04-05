#ifndef __MOVEMENT_QUANTA__
#define __MOVEMENT_QUANTA__

#include "cus_struct2.h"

#include "component_movement.h"

/* Building block for movemnt commands
*/
class MovementQuanta {
protected:
	// Internal cycle counter
	int cycle = -1;

	// Updates ComponentMovement with this MovementQuanta's state
	// Also returns a new position to be used (usually return input position)
	virtual Point3 updateExecution(shared_ptr<ComponentMovement>, const Point3&) = 0;

public:
	// Returns if this movement quanta is still executing
	virtual bool isExecuting() = 0;

public:
	virtual ~MovementQuanta() {
	}

	// Updates cycle and executes update
	Point3 update(shared_ptr<ComponentMovement> cMov, const Point3& position) {
		cycle++;
		return updateExecution(cMov, position);
	}

};



#endif