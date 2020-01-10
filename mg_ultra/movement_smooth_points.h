#ifndef __MOVEMENT_SMOOTH_POINTS__
#define __MOVEMENT_SMOOTH_POINTS__

#include <vector>
#include <glm/glm.hpp>
#include <tuple>

#include "algorithm_ex.h"
#include "movement_quanta.h"
#include "constants.h"

#include <iostream>

#define CR_CONST 0.5f

const glm::mat4 CR_BASIS(
	glm::vec4(0,            1.0f,            0,                   0        ),
	glm::vec4(-CR_CONST,    0,               CR_CONST,            0        ),
	glm::vec4(2 * CR_CONST, CR_CONST - 3.0f, 3.0f - 2 * CR_CONST, -CR_CONST),
	glm::vec4(-CR_CONST,    2.0f - CR_CONST, CR_CONST - 2.0f,     CR_CONST )
);

//allows for smoothed movement between points
class MovementSmoothPoints : public MovementQuanta {
	int duration;
	
	//interval start
	int timeStart = 0;
	//interval width
	int timeWidth = 0;

	//temporary input point storage
	vector<tuple<int, Point3>> inPoints;

	vector<tuple<int, Point3>> points;

	//specifies which spline is current lhs target
	int currentStep = 1;

	int timing = -1;
	
	void prep(shared_ptr<ComponentMovement> componentMovement) {
		componentMovement->pinVelocity();
		componentMovement->pushVelocityToPolar();
		componentMovement->setVelocity(Point3(0, 0, 0));
		componentMovement->setSpeed(0.0f);
	}

	//sets new interval
	void setNewInterval() {
		timeStart = get<0>(points[currentStep]);
		timeWidth = get<0>(points[currentStep + 1]) - timeStart;
	}

	//computes the current point
	Point3 computePoint(float s) {
		//glm::vec4 splineCoeff = glm::vec4(1.0f, s, SQUARE(s), CUBE(s));
		glm::vec4 splineCoeff = glm::vec4(1.0f,  s, SQUARE(s), CUBE(s));
		
		glm::mat4x3 controlVector = glm::mat4x3(
			get<1>(points[currentStep - 1]).getVec3(),
			get<1>(points[currentStep]).getVec3(),
			get<1>(points[currentStep + 1]).getVec3(),
			get<1>(points[currentStep + 2]).getVec3()
		);
		
		glm::vec3 interpretedPoint = (splineCoeff * transpose(CR_BASIS)) * transpose(controlVector);
		return Point3(interpretedPoint.x, interpretedPoint.y, interpretedPoint.z);
	}

public:

	MovementSmoothPoints(vector<tuple<int, Point3>>& inPoints) {
		this->inPoints = inPoints;
	}

	void setUpPoints(Point3 position) {
		//need to add starting and ending points
		Point3 start = (position * 2.0f) - get<1>(inPoints[0]);

		int n = this->inPoints.size() - 1;

		duration = get<0>(inPoints[n]);

		Point3 end = (get<1>(inPoints[n]) * 2.0f) - get<1>(inPoints[n - 1]);

		this->points.push_back(make_tuple(0, start));

		this->points.push_back(make_tuple(0, position));

		extend(this->points, inPoints);

		this->points.push_back(make_tuple(INT_MAX, end));

		inPoints.clear();
	}

	bool isExecuting() override {
		return timing < duration;
	}

	Point3 updateExecution(shared_ptr<ComponentMovement> componentMovement, const Point3& position) override {
		timing++;

		if (!timing) {
			prep(componentMovement);
			setUpPoints(position);
			//set up interval
			setNewInterval();
		}

		//increment current step if we reach the end of this segment
		if (get<0>(points[currentStep + 1]) <= timing) {
			currentStep++;
			setNewInterval();
		}

		float s = (float)((timing - timeStart) / (float)timeWidth);

		//compute point
		return currentStep + 2 == points.size() ? position : computePoint(s);
	}
};

#endif