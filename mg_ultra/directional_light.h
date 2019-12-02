/*A directional light unformly lights
from a given direction*/
#ifndef __DIRECTIONAL_LIGHT__
#define __DIRECTIONAL_LIGHT__

#include <mutex>
#include "cus_struct2.h"

struct DirectionalData {
	float direction[3];
	float colour[3];

	DirectionalData() {
	}

	DirectionalData(const Point3& direction, const Point3& colour) {
		this->direction[0] = direction.x;
		this->direction[1] = direction.y;
		this->direction[2] = direction.z;

		this->colour[0] = colour.x;
		this->colour[1] = colour.y;
		this->colour[2] = colour.z;
	}
};

class DirectionalLight {
	mutex directionLock;
	Point3 direction;
	mutex colourLock;
	Point3 colour;

public:
	DirectionalLight(const Point3& direction, const Point3& colour)
	: direction(direction), colour(colour) {

	}

	void setDirection(const Point3& direction) {
		unique_lock<mutex> lck(directionLock);
		this->direction = direction;
	}

	Point3 getDirection() {
		unique_lock<mutex> lck(directionLock);
		return direction;
	}

	void setColour(const Point3& colour) {
		unique_lock<mutex> lck(colourLock);
		this->colour = colour;
	}

	Point3 getColour() {
		unique_lock<mutex> lck(colourLock);
		return colour;
	}

	DirectionalData getData() {
		unique_lock<mutex> clck(directionLock);
		return DirectionalData(direction, colour);
	}
};



#endif