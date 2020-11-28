/** 
 * Static movement allow scripting entity movement
 * such that the position of the entity can
 * be determined by a function of time and surronding entity condition (i.e. player position)
 */
#ifndef __MOVEMENT_COMMANDER__
#define __MOVEMENT_COMMANDER__

#include <memory>

#include "error.h"
#include "movement_command.h"

// Allows for a list of movement commands
class MovementCommander {
	int currentTime = -1;
	MMap<int, MovementCommand> movementCommands;

	// Movement commadns still in affect
	vector<MovementCommand> executingMovementCommands;

	// Last smooth style movement command
	int lastSmooth = 0;

	static void movementCommandClear(MovementCommand& movementCommand) {
		movementCommand.free();
	}

public:

	~MovementCommander() {
		movementCommands.execute(&MovementCommander::movementCommandClear);
	}

	// Adds a fixed point movement
	void addFixedPoint(int time, const Point3& position) {
		movementCommands.add(
			time, 
			MovementCommand::createFixedPoint(position)
		);
	}

	// Chains a smooth point movement, looking for the latest smooth style input
	// as the starting point, and time as the end point
	void chainFixedMove(int time, const Point3& position) {
		movementCommands.add(
			lastSmooth, 
			MovementCommand::createFixedMove(position, time - lastSmooth)
		);
		lastSmooth = time;
	}

	// Creates a new smooth point
	void chainSmoothPoints(vector<float>& input) {
		if (input.size() % 4) {
			err::logMessage("Invalid list of inputs");
		}

		vector<tuple<int, Point3>> points;
		int last = 0;

		for (int i = 0; i < (int)input.size(); i+=4) {
			points.push_back(
				make_tuple(
					(int)input[i], 
					Point3(input[i + 1], input[i + 2], input[i + 3])
				)
			);
			last = (int)input[i];
		}

		movementCommands.add(
			lastSmooth,
			MovementCommand::createSmoothPoints(points)
		);
		lastSmooth = last;
	}

	// Add a polar laumch
	void addPolarLaunch(int time, float mag, float ang) {
		movementCommands.add(
			time,
			MovementCommand::createPolarLaunch(mag, ang)
		);
	}

	// Add a polar turn
	void addPolarturn(int time, int duration, float totalAngle) {
		movementCommands.add(
			time,
			MovementCommand::createPolarTurn(duration, totalAngle)
		);
	}
	
	// Add a polar accelerate to
	void addPolarAccelerateTo(int time, int duration, float endingSpeed) {
		movementCommands.add(
			time,
			MovementCommand::createPolarAccelerateTo(duration, endingSpeed)
		);
	}

	// Add point acceleration
	void addPointAccelerateTo(int time, const Point3& acceleration) {
		movementCommands.add(
			time,
			MovementCommand::createPointAccelerate(acceleration)
		);
	}

	Point3 getUpdatedPosition(shared_ptr<ComponentMovement> componentMovement, Point3 position) {
		currentTime++;

		// Add any new movement commands
		if (movementCommands.count(currentTime)) {
			extend(executingMovementCommands, movementCommands[currentTime]);
		}

		// Iterate over executing movement commands, deleting where necessary
		auto it = executingMovementCommands.begin();
		while (it != executingMovementCommands.end()) {
			position = it->update(componentMovement, position);
			it = it->isExecuting() ? ++it : executingMovementCommands.erase(it);
		}
		
		return position;
	}
};


#endif