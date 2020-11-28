#ifndef __MOVEMENT_COMMAND__
#define __MOVEMENT_COMMAND__

#include "mmap.h"

#include "movement_fixed_point.h"
#include "movement_fixed_move.h"
#include "movement_smooth_points.h"
#include "movement_polar_launch.h"
#include "movement_polar_turn.h"
#include "movement_polar_accelerate_to.h"
#include "movement_point_accelerate.h"

// Base class that represents a single movement quanta
// Movement command at a given time
class MovementCommand {
	MovementQuanta* internalQuanta = nullptr;

	MovementCommand() {

	}

public:
	void free() {
		delete internalQuanta;
	}

	// Specifies if this command is still executing
	bool isExecuting() {
		return internalQuanta->isExecuting();
	}

	// Updates movement command and position
	Point3 update(shared_ptr<ComponentMovement> componentMovement, const Point3& position) {
		return internalQuanta->update(componentMovement, position);
	}

	// Named constructors for creating new MovementCommands
	static MovementCommand createFixedPoint(const Point3& position) {
		MovementCommand movementCommand;
		movementCommand.internalQuanta = new MovementFixedPoint(position);
		return movementCommand;
	}

	// Adds a smooth point
	static MovementCommand createFixedMove(const Point3& position, int duration) {
		MovementCommand movementCommand;
		movementCommand.internalQuanta = new MovementFixedMove(position, duration);
		return movementCommand;
	}

	// Adds a smooth point
	static MovementCommand createSmoothPoints(vector<tuple<int, Point3>>& points) {
		MovementCommand movementCommand;
		movementCommand.internalQuanta = new MovementSmoothPoints(points);
		return movementCommand;
	}


	// Add a polar launch
	static MovementCommand createPolarLaunch(float mag, float ang) {
		MovementCommand movementCommand;
		movementCommand.internalQuanta = new MovementPolarLaunch(Point2(mag, ang));
		return movementCommand;
	}

	// Add a polar turn
	static MovementCommand createPolarTurn(int duration, float totalTurn) {
		MovementCommand movementCommand;
		movementCommand.internalQuanta = new MovementPolarTurn(totalTurn / duration, duration);
		return movementCommand;
	}

	// Add a polar speed shift
	static MovementCommand createPolarAccelerateTo(int duration, float endingSpeed) {
		MovementCommand movementCommand;
		movementCommand.internalQuanta = new MovementPolarAccelerateTo(endingSpeed, duration);
		return movementCommand;
	}

	// Add a point acceleration
	static MovementCommand createPointAccelerate(const Point3& acceleration) {
		MovementCommand movementCommand;
		movementCommand.internalQuanta = new MovementPointAccelerate(acceleration);
		return movementCommand;
	}
};


#endif