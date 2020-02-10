#ifndef __MOVEMENT_COMMAND__
#define __MOVEMENT_COMMAND__

#include "mmap.h"

#include "movement_fixed_point.h"
#include "movement_fixed_move.h"
#include "movement_smooth_points.h"
#include "movement_polar_launch.h"

//Base class that represents a single movement quanta
//Movement command at a given time
class MovementCommand {
	MovementQuanta* internalQuanta = nullptr;

	MovementCommand() {

	}

public:
	void free() {
		delete internalQuanta;
	}

	//specifies if this command is still executing
	bool isExecuting() {
		return internalQuanta->isExecuting();
	}

	//updates movement command and position
	Point3 updateExecution(shared_ptr<ComponentMovement> componentMovement, const Point3& position) {
		return internalQuanta->updateExecution(componentMovement, position);
	}

	//named constructors for creating new MovementCommands
	static MovementCommand createFixedPoint(const Point3& position) {
		MovementCommand movementCommand;
		movementCommand.internalQuanta = new MovementFixedPoint(position);
		return movementCommand;
	}

	//adds a smooth point
	static MovementCommand createFixedMove(const Point3& position, int duration) {
		MovementCommand movementCommand;
		movementCommand.internalQuanta = new MovementFixedMove(position, duration);
		return movementCommand;
	}

	//adds a smooth point
	static MovementCommand createSmoothPoints(vector<tuple<int, Point3>>& points) {
		MovementCommand movementCommand;
		movementCommand.internalQuanta = new MovementSmoothPoints(points);
		return movementCommand;
	}


	//add a polar launch
	static MovementCommand createPolarLaunch(float mag, float ang) {
		MovementCommand movementCommand;
		movementCommand.internalQuanta = new MovementPolarLaunch(Point2(mag, ang));
		return movementCommand;
	}
};


#endif